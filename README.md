# Edge-Optimized Semantic Knowledge Graph

**Zero-Allocation | Bare-Metal C++ | RISC-V Optimized**

This repository contains the Semantic Knowledge Graph subsystem for our Genesys-2 Hardware/Software Co-Design project. It bridges the semantic gap between unpredictable human natural language prompts and strict bounding-box target classes (COCO) required by the YOLOE Nano vision accelerator.

By fundamentally decoupling semantic routing from visual processing, this architecture achieves "open-vocabulary" flexibility without the massive power, thermal, and memory bottlenecks of deploying large Vision-Language Models (VLMs) on the edge.

## 🚀 Key Engineering Features

- **Zero Heap Allocation:** The C++ engine (`kg_lookup.h`) uses strictly stack-based arrays and `constexpr` tables. It never calls `malloc` or `new`, guaranteeing zero memory leaks and absolute stability on the VEGA soft-core.
- **O(1) Hash Lookups:** Replaces sequential Graph/Tree traversal with inline `djb2` string hashing, reducing semantic resolution time to under 500 nanoseconds.
- **Build-Time Code Generation:** JSON ontology parsing is handled _offline_ by `codegen.py`. The hardware only reads pre-resolved, static read-only memory (`.rodata`).
- **Memory Protection:** Implements hard character limits and strict buffer boundaries before any string processing occurs.

## 📂 Repository Structure

```text
dvcon/
├── edge_kg.py                 # Python runtime/sandbox (laptop/server)
├── codegen.py                 # Build-time C++ header generator
├── test_kg_header.cpp         # C++ test harness
├── kg_config/
│   ├── ontology.json          # Core Action -> COCO class mappings
│   └── synonym_map.json       # User input aliases (e.g., "sip" -> "drink")
├── include/
│   ├── kg_lookup.h            # Bare-metal C++ library (The Engine)
│   └── kg_data.h              # Generated static data (Do not edit manually)
└── tests/
    └── test_edge_kg.py        # Python unit tests
```

## 🛠️ Architecture Workflow

This system utilizes an asymmetric build-and-deploy pipeline to maximize hardware efficiency.

### 1. Configuration (The Data Layer)

Semantic relationships are defined in standard JSON format inside `kg_config/`.

- To add new tasks or target classes, simply update `ontology.json`.
- To teach the system new vocabulary words without duplicating data, add them to `synonym_map.json`.

### 2. Code Generation (The Build Layer)

The VEGA processor does not parse JSON. Before compiling your embedded C++ code, run the code generator on your host machine:

```bash
python codegen.py
```

## 🧪 Running the Tests

To run the C++ bare-metal simulation natively on your machine:

```bash
g++ test_kg_header.cpp -o test_kg -std=c++17 -fno-exceptions -fno-rtti
./test_kg
```

To test the Python sandbox:

```bash
pytest tests/test_edge_kg.py
```
