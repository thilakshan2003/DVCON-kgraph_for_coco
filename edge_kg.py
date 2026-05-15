"""Edge Knowledge Graph Implementation
A lightweight, deterministic semantic knowledge graph for edge processors.
This refactor moves ontology and synonyms to JSON config files, adds
synonym aliasing, and enforces prompt truncation for safety.
"""

import re
import json
import os
import argparse
import logging

LOGGER = logging.getLogger(__name__)


class EdgeKnowledgeGraph:
    """Deterministic KG mapping action tokens to ordered COCO class lists."""

    def __init__(self, config_dir=None, max_prompt_chars=100):
        """Load ontology, stop words and synonym map from config (if provided).

        Args:
            config_dir (str|None): Path to directory containing JSON configs.
            max_prompt_chars (int): Hard truncation limit for incoming prompts.
        """
        self.max_prompt_chars = int(max_prompt_chars) if max_prompt_chars else 100

        # Default ontology and stop words (fallbacks)
        self.ontology_graph = {
            "eat": ["apple", "banana", "orange", "sandwich", "pizza", "fork", "plate"],
            "drink": ["cup", "wine glass", "bottle", "glass", "water bottle"],
            "work": ["desk", "laptop", "keyboard", "mouse", "chair", "person"],
            "cook": ["oven", "pot", "pan", "knife", "stove", "fork"],
            "play": ["ball", "frisbee", "bat", "racket", "person", "sports"],
            "read": ["book", "laptop", "keyboard", "person", "chair", "glasses"],
            "wear": ["hat", "shoes", "backpack", "glasses", "belt", "pants", "shirt"],
            "throw": ["ball", "frisbee", "person", "hand"],
            "ride": ["bicycle", "motorcycle", "car", "person", "helmet"],
            "sit": ["chair", "bench", "couch", "bed", "person", "sofa"],
            "hold": ["cup", "bottle", "phone", "book", "bag", "person"],
            "walk": ["person", "dog", "shoe", "street", "road", "sidewalk"],
        }

        self.stop_words = {
            "something",
            "to",
            "find",
            "me",
            "a",
            "an",
            "the",
            "from",
            "in",
            "on",
            "at",
            "and",
            "or",
            "is",
            "am",
            "are",
            "be",
            "been",
            "being",
            "have",
            "has",
            "had",
            "do",
            "does",
            "did",
            "will",
            "would",
            "could",
            "should",
            "may",
            "might",
            "can",
            "for",
            "of",
            "with",
            "that",
            "this",
        }

        # Synonym alias map: alias -> canonical token
        self.synonym_map = {}

        # Keep config dir and mtimes for hot-reload
        self.config_dir = config_dir
        self._config_mtimes = {}

        # Try to load external configs
        if config_dir:
            try:
                self._load_configs(config_dir)
            except Exception as e:
                LOGGER.warning("Failed to load config from %s: %s", config_dir, e)

    def _load_json_file(self, path):
        with open(path, "r", encoding="utf-8") as fh:
            return json.load(fh)

    def _load_configs(self, config_dir):
        """Load `ontology.json` and `synonym_map.json` from `config_dir` if present."""
        # Ontology file may contain keys: ontology_graph, stop_words
        ontology_path = os.path.join(config_dir, "ontology.json")
        if os.path.isfile(ontology_path):
            data = self._load_json_file(ontology_path)
            # Optional: validate schema if jsonschema is available
            try:
                import jsonschema

                schema = {
                    "type": "object",
                    "properties": {
                        "ontology_graph": {
                            "type": "object",
                            "additionalProperties": {"type": "array", "items": {"type": "string"}},
                        },
                        "stop_words": {"type": "array", "items": {"type": "string"}},
                    },
                    "required": ["ontology_graph"],
                }
                jsonschema.validate(instance=data, schema=schema)
            except Exception:
                # If validation fails or jsonschema missing, proceed but warn
                LOGGER.debug("ontology.json did not validate or jsonschema unavailable")

            if isinstance(data, dict):
                if "ontology_graph" in data and isinstance(data["ontology_graph"], dict):
                    self.ontology_graph = data["ontology_graph"]
                if "stop_words" in data and isinstance(data["stop_words"], list):
                    self.stop_words = set(data["stop_words"])

            # record mtime
            try:
                self._config_mtimes[ontology_path] = os.path.getmtime(ontology_path)
            except Exception:
                pass

        synonym_path = os.path.join(config_dir, "synonym_map.json")
        if os.path.isfile(synonym_path):
            sm = self._load_json_file(synonym_path)
            # Validate simple mapping shape if possible
            try:
                import jsonschema

                schema_syn = {"type": "object", "additionalProperties": {"type": "string"}}
                jsonschema.validate(instance=sm, schema=schema_syn)
            except Exception:
                LOGGER.debug("synonym_map.json did not validate or jsonschema unavailable")

            if isinstance(sm, dict):
                self.synonym_map = sm

            try:
                self._config_mtimes[synonym_path] = os.path.getmtime(synonym_path)
            except Exception:
                pass

    def _get_mtime(self, path):
        try:
            return os.path.getmtime(path) if os.path.isfile(path) else None
        except Exception:
            return None

    def _maybe_reload_configs(self):
        """Reload configs if their files have changed on disk (hot-reload)."""
        if not self.config_dir:
            return

        ontology_path = os.path.join(self.config_dir, "ontology.json")
        synonym_path = os.path.join(self.config_dir, "synonym_map.json")

        reload_needed = False

        m_ont = self._get_mtime(ontology_path)
        prev_ont = self._config_mtimes.get(ontology_path)
        if m_ont and prev_ont != m_ont:
            reload_needed = True

        m_syn = self._get_mtime(synonym_path)
        prev_syn = self._config_mtimes.get(synonym_path)
        if m_syn and prev_syn != m_syn:
            reload_needed = True

        if reload_needed:
            try:
                self._load_configs(self.config_dir)
                LOGGER.info("Configs reloaded from %s", self.config_dir)
            except Exception as e:
                LOGGER.warning("Failed to reload configs: %s", e)

    def _clean_prompt(self, prompt_string):
        """Lowercase, strip punctuation and filter stop words."""
        cleaned = prompt_string.lower()
        cleaned = re.sub(r"[^\w\s-]", " ", cleaned)
        words = cleaned.split()
        keywords = [word for word in words if word not in self.stop_words and word.strip()]
        return keywords

    def resolve_prompt(self, prompt_string):
        """Truncate, clean, map synonyms, and resolve tokens to COCO classes."""
        # Check for config file changes (hot-reload)
        self._maybe_reload_configs()

        # Enforce hard truncation for safety
        if not isinstance(prompt_string, str):
            prompt_string = str(prompt_string or "")

        truncated = prompt_string[: self.max_prompt_chars]

        # Extract cleaned keywords
        keywords = self._clean_prompt(truncated)

        target_classes = []

        for keyword in keywords:
            # Map via synonym pointers first
            canonical = self.synonym_map.get(keyword, keyword)
            if canonical in self.ontology_graph:
                target_classes.extend(self.ontology_graph[canonical])

        # Deduplicate while preserving order
        deduplicated = list(dict.fromkeys(target_classes))
        return deduplicated


def main():
    parser = argparse.ArgumentParser(description="EdgeKnowledgeGraph interactive demo")
    parser.add_argument("--config-dir", help="Path to kg_config directory", default=None)
    parser.add_argument("--max-chars", help="Max prompt characters (hard truncate)", type=int, default=100)
    args = parser.parse_args()

    kg = EdgeKnowledgeGraph(config_dir=args.config_dir, max_prompt_chars=args.max_chars)

    print("=" * 60)
    print("Edge Knowledge Graph - Interactive Demo")
    print("=" * 60)
    print("\nThis tool converts natural language prompts to COCO classes.")
    print("Example actions: eat, drink, work, cook, play, read, wear,")
    print("                 throw, ride, sit, hold, walk")
    print("\nType 'quit' or 'exit' to stop.\n")

    while True:
        try:
            user_input = input("Enter a prompt (or 'quit' to exit): ").strip()
            if user_input.lower() in ("quit", "exit", "q"):
                print("\nGoodbye!")
                break
            if not user_input:
                print("Please enter a prompt.\n")
                continue

            result = kg.resolve_prompt(user_input)

            print(f"\nInput: '{user_input}'")
            print(f"Cleaned Keywords: {kg._clean_prompt(user_input)}")
            if result:
                print(f"Target COCO Classes: {result}")
            else:
                print("Target COCO Classes: [] (no matches found)")
            print()

        except KeyboardInterrupt:
            print("\n\nInterrupted. Goodbye!")
            break


if __name__ == "__main__":
    main()
