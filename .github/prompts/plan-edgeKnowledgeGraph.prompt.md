# Plan: Edge Knowledge Graph Implementation

**TL;DR**  
Build a lightweight `EdgeKnowledgeGraph` class using only Python stdlib that converts natural language prompts into COCO dataset object classes. Uses deterministic keyword matching against an ontology of action verbs, maintains priority-ordered outputs, and runs an interactive prompt loop where each query is independent.

---

## Steps

### Phase 1: Environment Setup (Terminal)
1. Create Python virtual environment (`kg_env`) in workspace root using `python -m venv kg_env`
2. Activate it with `.\kg_env\Scripts\activate` (Windows)
3. No pip installs needed — standard library only

### Phase 2: Core Implementation (`edge_kg.py`)
1. Create `EdgeKnowledgeGraph` class with:
   - `__init__` method to initialize `ontology_graph` (dict) and `stop_words` (set)
   
2. Populate `ontology_graph` with 10+ action verbs/nouns → COCO classes:
   - Example actions: eat, drink, work, cook, play, search, build, read, wear, throw, ride, sit
   - Example mappings: "eat" → ["apple", "banana", "sandwich", "pizza", "fork"]
   - Values must be strictly ordered lists (priority-based)
   
3. Populate `stop_words` with common filler words: "something", "to", "find", "me", "a", "an", "the", "from", "in", "on"

4. Implement `_clean_prompt(prompt_string)` helper:
   - Convert to lowercase
   - Remove punctuation using `re.sub()` (keep internal hyphens)
   - Split on whitespace
   - Filter out stop words
   - Return list of extracted keywords

5. Implement `resolve_prompt(prompt_string)` main method:
   - Call `_clean_prompt()` to extract keywords
   - Iterate through keywords in order
   - For each keyword, check if it exists in `ontology_graph`
   - Append matched COCO classes to result list
   - Deduplicate **globally** while maintaining original priority order (first occurrence wins)
   - Return final list (or empty list if no matches)

6. Implement interactive loop in `if __name__ == "__main__"`:
   - Use `input()` to prompt user for phrase
   - Each prompt is independent (no state carried between)
   - Call `resolve_prompt()` and print results
   - Loop until user exits (e.g., enters "quit" or "exit")
   - Include 3 example phrases in comments as reference

---

## Relevant Files
- `edge_kg.py` — Main implementation (to be created)
- `kg_env/` — Virtual environment directory (to be created)

---

## Verification

1. **Virtual Environment**
   - Confirm `kg_env` folder exists
   - Confirm activation script runs without errors
   
2. **Code Structure**
   - Verify `EdgeKnowledgeGraph` class exists with all required methods
   - Verify `ontology_graph` has 10+ entries with ordered lists
   - Verify `stop_words` set is populated
   
3. **Functionality Tests**
   - Test phrase: "Something to drink tea from" → Should match "drink" action, return cup-related classes
   - Test phrase: "eat apple" → Should match "eat" action, return apple (if in ontology)
   - Test phrase: "nothing matches here xyz" → Should return empty list
   - Verify deduplication works (run two phrases with overlapping classes)
   - Verify order is preserved (first occurrence of each class wins)

4. **Interactive Loop**
   - Run script, manually enter test phrases
   - Verify output is returned and loop continues
   - Verify "quit" or "exit" terminates cleanly

---

## Decisions Made
- **Only one independent query per prompt**: Each call to `resolve_prompt()` is stateless and returns immediately
- **Global deduplication**: If "cup" appears in both "eat" and "drink" actions, it's returned once (at first occurrence position)
- **Keyword order from prompt**: Keywords are matched in the order they appear in the cleaned prompt
- **No fuzzy matching**: Exact string matching only (`in ontology_graph`)
- **Hyphenated words intact**: Words like "tea-cup" aren't split; "tea cup" (two words) split normally
- **Interactive mode**: User inputs one prompt at a time; each resolves independently then program waits for next input

---

## Further Considerations
1. **COCO dataset realism** — Implementation should include verified COCO classes (person, cup, bottle, apple, etc.) for accuracy
2. **Deduplication algorithm** — Use dict.fromkeys() pattern to preserve insertion order while removing duplicates (Python 3.7+ guaranteed ordering)
3. **Edge case: empty prompt** — Returns empty list (no keywords match, which is correct)
4. **Case sensitivity** — All matching is case-insensitive (converted to lowercase)
