# Agent and Developer Contributing Guidelines

This document establishes the guidelines, conventions, and architectural standards for contributing to the **Dijkstra** project. All developers and automated AI agents must follow these rules.

---

## 1. Project Philosophy & Objectives

- **Educational & Pedagogical**: The codebase serves as a high-quality reference implementation of graph data structures, shortest-path algorithms (Dijkstra), and modern C++ design patterns. Explanations in code, comments, and documentation must be clear, rigorous, and approachable.
- **Clean Architecture**: Strong decoupling between the reusable core library (`dijkstra-lib`) and the user-facing CLI tools. The core library must have zero dependencies on terminal I/O or CLI argument parsers, making it ready for any frontend (CLI, WebAssembly, GUI, TUI).
- **Safety and Modern Standards**: Modern C++ (C++20), strict compiler warnings, memory sanitizers (ASan/UBSan), and robust exception handling.

---

## 2. Language & Git Conventions

### 2.1 Language
- **Source Code**: All code, identifiers, types, variables, comments, docstrings, error messages, and commit messages **must be written in English**.
- **User Interactions**: Agent communication with the user respects the user's preferred language (e.g., Spanish).

### 2.2 Conventional Commits
All commits must follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<optional scope>): <description>

[optional body]

[optional footer(s)]
```

#### Allowed Types:
- `feat`: A new feature or capability (e.g., `feat: implement DijkstraResult and early exit`).
- `fix`: A bug fix (e.g., `fix: prevent division by zero in point normalization`).
- `docs`: Documentation only changes (e.g., `docs: add algorithm visual walkthrough with Mermaid`).
- `style`: Formatting, whitespace, clang-format adjustments with no code logic changes.
- `refactor`: Code change that neither fixes a bug nor adds a feature.
- `perf`: A code change that improves performance.
- `test`: Adding missing tests or correcting existing tests.
- `build`: Changes that affect the build system (CMake, dependencies).
- `ci`: Changes to CI configuration files and scripts (GitHub Actions).
- `chore`: Maintenance tasks, repo tooling, or scaffolding changes.

### 2.3 Atomic Commits & Amendments
- **One task, one commit**: Every discrete step or feature must be committed separately.
- **Commit Amend / Squash**: If a correction or refinement is needed for a step just performed on the current branch, use `git commit --amend` (or squash) to preserve a clean, linear, and meaningful git history.

---

## 3. Directory Layout & Scaffolding

```
.
├── CMakeLists.txt                 # Root CMake build configuration
├── CMakePresets.json              # Standard CMake presets (debug, release, asan)
├── AGENTS.md                      # Developer and agent guidelines (this file)
├── LICENSE                        # MIT License
├── README.md                      # Project landing page and overview
├── mkdocs.yml                     # Documentation site configuration (MkDocs Material)
├── .clang-format                  # Code formatting style rules
├── .github/
│   └── workflows/
│       ├── cmake.yml              # CI workflow (build, test, sanitizers)
│       └── static.yml             # Documentation deployment to GitHub Pages
├── docs/                          # Pedagogical Markdown documentation + Mermaid
│   ├── index.md                   # Documentation home
│   ├── algorithm.md               # Dijkstra algorithm explanation & step trace
│   ├── data-structures.md         # Graph representations & priority queues
│   ├── cpp-features.md            # C++20 modern features reference
│   ├── cli.md                     # CLI usage guide and Graphviz visualization
│   └── testing.md                 # Testing strategy and sanitizers guide
├── include/
│   └── dijkstra/                  # Public library headers
│       ├── types.h                # NodeId, Weight, Edge, constants
│       ├── exceptions.h           # Custom domain exception hierarchy
│       ├── point.h                # 2D Point structure & Euclidean helpers
│       ├── graph.h                # Graph class interface & generators
│       └── dijkstra.h             # shortest_paths, shortest_path, DijkstraResult
├── src/
│   ├── library/                   # Core library implementation (dijkstra-lib)
│   │   ├── CMakeLists.txt
│   │   ├── point.cpp
│   │   ├── graph.cpp
│   │   └── dijkstra.cpp
│   ├── dijkstra/                  # dijkstra CLI application
│   │   ├── CMakeLists.txt
│   │   └── main.cpp
│   └── random-graph/              # random-graph generator CLI application
│       ├── CMakeLists.txt
│       └── main.cpp
└── test/
    ├── CMakeLists.txt             # GoogleTest test runner configuration
    ├── unit/                      # Unit tests
    │   ├── point_test.cpp
    │   ├── graph_test.cpp
    │   ├── dijkstra_test.cpp
    │   └── io_test.cpp
    └── integration/               # CLI and end-to-end smoke tests
        └── cli_test.sh
```

---

## 4. C++20 Coding Standards

1. **Namespaces**:
   - All library code belongs to `namespace dijkstra`.
   - Never use `using namespace std;` in header files. Avoid global `using namespace std;` in `.cpp` files.

2. **Types & Precision**:
   - Use `dijkstra::NodeId` (`std::size_t`) for node identifiers.
   - Use `dijkstra::Weight` (`double`) for edge weights and path distances.
   - Use `std::optional<T>` to express optional presence (e.g., path existence, seeds).
   - Use `std::span` or `const std::vector<T>&` for read-only contiguous sequences.

3. **Attributes & Const-Correctness**:
   - Annotate return values that should not be ignored with `[[nodiscard]]`.
   - Mark non-mutating member functions as `const`.
   - Mark single-argument constructors as `explicit` when appropriate.

4. **Error Handling**:
   - **Never** call `std::exit()` or `std::abort()` inside library code.
   - Throw specialized domain exceptions inheriting from `dijkstra::DijkstraException` (e.g., `GraphParseException`, `InvalidNodeException`, `NegativeWeightException`).

5. **Memory & Ownership**:
   - Value semantics and RAII by default.
   - Avoid raw pointer ownership; use standard containers (`std::vector`, `std::priority_queue`).

---

## 5. Testing & Quality Assurance

1. **Unit Tests (GoogleTest)**:
   - Every class, function, and edge case must be tested.
   - Required scenarios:
     - Canonical sample graph (Wikipedia Dijkstra example).
     - Disconnected components / unreachable nodes.
     - Single-node graphs.
     - Cyclic graphs and multiple paths between the same nodes.
     - Zero-weight edges.
     - Target-specific search with early termination.
     - Corrupted or invalid input stream parsing (exception verification).

2. **Integration & Smoke Tests**:
   - Validate CLI executables through automated shell scripts registered with CTest.
   - Validate pipe workflows: `random-graph | dijkstra`.
   - Validate command-line options (`--help`, `--start`, `--target`, `--seed`, etc.).

3. **Sanitizers**:
   - Code must compile and pass all tests cleanly under AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan).

---

## 6. Documentation Standards

- Documentation is authored in Markdown within `docs/` and rendered using MkDocs Material.
- Use **Mermaid** diagrams (`mermaid` code blocks) to visually illustrate:
  - Graph structures and representations.
  - Step-by-step priority queue iterations during shortest path discovery.
  - Architecture and component relationships.
- Equations should use standard LaTeX notation rendered via MathJax/KaTeX.
