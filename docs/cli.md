# Command-Line Interfaces (CLI) Reference

The project includes two command-line tools: `random-graph` for graph generation and `dijkstra` for shortest-path calculation.

---

## 1. `dijkstra` Application

The `dijkstra` executable calculates shortest paths from an input file or standard input.

```bash
dijkstra [OPTIONS] [FILE]
```

### Options

| Option | Long Flag | Description | Default |
|:---|:---|:---|:---|
| `-s` | `--start <NODE>` | Source node index | `0` |
| `-t` | `--target <NODE>` | Target destination node (enables early exit) | All reachable nodes |
| `-f` | `--format <FORMAT>` | Output format: `text`, `dot`, `json` | `text` |
| `-b` | `--benchmark` | Measure and print runtime in milliseconds | Disabled |
| `-q` | `--quiet` | Suppress info messages on stderr | Disabled |
| `-h` | `--help` | Show usage screen and exit | — |

---

## 2. `random-graph` Application

Generates synthetic random graphs using 2D geometric or Erdős-Rényi models.

```bash
random-graph [OPTIONS] [SIZE [CONNECTIONS]]
```

### Options

| Option | Long Flag | Description | Default |
|:---|:---|:---|:---|
| `-n` | `--nodes <N>` | Total number of vertices | `100` |
| `-k` | `--connections <K>` | Closest neighbors per node (geometric model) | `3` |
| `-p` | `--prob <PROB>` | Edge probability for Erdős-Rényi model ($0.0 \dots 1.0$) | Disabled (geometric) |
| `-s` | `--seed <SEED>` | Deterministic 64-bit random seed | Random device |
| `-f` | `--format <FORMAT>` | Output format: `matrix`, `dot` | `matrix` |
| `-q` | `--quiet` | Suppress progress messages | Disabled |
| `-h` | `--help` | Show usage screen | — |

---

## 3. Workflows & Recipes

### 3.1 Piped Execution
Generate a 10-node graph and calculate shortest paths from node 0:
```bash
random-graph -n 10 -k 3 -s 42 | dijkstra -s 0
```

### 3.2 Finding the Path to a Specific Target
Compute the path and distance specifically from node 0 to node 7:
```bash
random-graph -n 10 -k 3 -s 42 | dijkstra -s 0 -t 7
```
Output:
```text
Shortest path from 0 to 7 (distance: 1.15783):
0 → 3 → 7
```

### 3.3 Exporting Structured JSON
```bash
random-graph -n 5 -k 2 -s 123 | dijkstra -s 0 -f json
```

### 3.4 Visualizing with Graphviz (DOT $\to$ PNG)
Export graph to DOT format and render using Graphviz `dot`:
```bash
random-graph -n 6 -k 2 -f dot > graph.dot
dot -Tpng graph.dot -o graph.png
```
