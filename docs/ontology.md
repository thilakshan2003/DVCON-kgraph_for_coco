# Ontology & Synonym Configs

Place JSON configuration files under `kg_config/` and point `edge_kg.py` to that
directory with `--config-dir`. Two files are recognized:

- `ontology.json` — contains `ontology_graph` (mapping of canonical tokens to COCO classes)
  and `stop_words` (array of tokens to ignore).
- `synonym_map.json` — flat map of alias -> canonical token (e.g., `"sip": "drink"`).

The Python implementation loads these files at startup and falls back to built-in
defaults when files are missing. `max_prompt_chars` enforces a hard truncation to
protect constrained runtimes.

For C++ migration: map ontology keys to integer ids (`std::unordered_map<std::string,int>`)
and store adjacency lists as `std::vector<std::vector<int>>` for minimal memory.
