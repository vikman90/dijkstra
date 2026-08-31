#!/usr/bin/env bash
set -euo pipefail

# CLI Integration and Smoke Test Suite for Dijkstra and Random-Graph

DIJKSTRA_BIN="${1:-./build/src/dijkstra/dijkstra}"
RANDOM_GRAPH_BIN="${2:-./build/src/random-graph/random-graph}"

echo "Testing binaries:"
echo "  dijkstra:     $DIJKSTRA_BIN"
echo "  random-graph: $RANDOM_GRAPH_BIN"

if [[ ! -x "$DIJKSTRA_BIN" ]]; then
    echo "Error: dijkstra binary not found or not executable at $DIJKSTRA_BIN"
    exit 1
fi

if [[ ! -x "$RANDOM_GRAPH_BIN" ]]; then
    echo "Error: random-graph binary not found or not executable at $RANDOM_GRAPH_BIN"
    exit 1
fi

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

echo "=== Test 1: Help flags ==="
"$DIJKSTRA_BIN" --help > /dev/null
"$DIJKSTRA_BIN" -h > /dev/null
"$RANDOM_GRAPH_BIN" --help > /dev/null
"$RANDOM_GRAPH_BIN" -h > /dev/null
echo "Passed: Help flags exit cleanly."

echo "=== Test 2: Invalid options rejection ==="
if "$DIJKSTRA_BIN" --invalid-nonexistent-flag 2> /dev/null; then
    echo "Failed: dijkstra accepted invalid flag"
    exit 1
fi
if "$RANDOM_GRAPH_BIN" --invalid-flag 2> /dev/null; then
    echo "Failed: random-graph accepted invalid flag"
    exit 1
fi
echo "Passed: Invalid flags rejected."

echo "=== Test 3: Standard pipeline integration ==="
OUTPUT="$("$RANDOM_GRAPH_BIN" -n 8 -k 3 -s 42 -q | "$DIJKSTRA_BIN" -s 0 -q)"
if [[ -z "$OUTPUT" ]]; then
    echo "Failed: Pipeline produced empty output"
    exit 1
fi
echo "$OUTPUT" | grep -q "→ 0"
echo "Passed: Pipeline works correctly."

echo "=== Test 4: Single target path search ==="
TARGET_OUTPUT="$("$RANDOM_GRAPH_BIN" -n 10 -k 3 -s 42 -q | "$DIJKSTRA_BIN" -s 0 -t 3 -q)"
echo "$TARGET_OUTPUT" | grep -q "Shortest path from 0 to 3"
echo "Passed: Target-specific search works."

echo "=== Test 5: JSON format output ==="
JSON_OUT="$("$RANDOM_GRAPH_BIN" -n 6 -k 2 -s 10 -q | "$DIJKSTRA_BIN" -s 0 -f json -q)"
python3 -c "import json, sys; data = json.loads('''$JSON_OUT'''); assert data['source'] == 0; assert len(data['results']) == 6"
echo "Passed: JSON format output is valid."

echo "=== Test 6: DOT format output ==="
DOT_OUT="$("$RANDOM_GRAPH_BIN" -n 5 -k 2 -f dot -q)"
echo "$DOT_OUT" | grep -q "graph RandomGraph"
echo "Passed: DOT format export is valid."

echo "=== Test 7: File input handling ==="
SAMPLE_FILE="$TMP_DIR/sample.txt"
printf "7 9 0 0 14\n10 15 0 0\n11 0 2\n6 0\n9\n" > "$SAMPLE_FILE"
FILE_OUT="$("$DIJKSTRA_BIN" -s 0 -q "$SAMPLE_FILE")"
STDIN_OUT="$("$DIJKSTRA_BIN" -s 0 -q < "$SAMPLE_FILE")"
if [[ "$FILE_OUT" != "$STDIN_OUT" ]]; then
    echo "Failed: File input and stdin input produced different outputs"
    exit 1
fi
echo "Passed: File input behaves identically to stdin."

echo "=== Test 8: Non-existent file error handling ==="
if "$DIJKSTRA_BIN" "$TMP_DIR/does_not_exist.txt" 2> /dev/null; then
    echo "Failed: dijkstra should have returned error on non-existent file"
    exit 1
fi
echo "Passed: Non-existent file handled properly."

echo "=== Test 9: Malformed input file error handling ==="
MALFORMED_FILE="$TMP_DIR/bad.txt"
printf "7 9 INVALID 0 14\n" > "$MALFORMED_FILE"
if "$DIJKSTRA_BIN" "$MALFORMED_FILE" 2> /dev/null; then
    echo "Failed: dijkstra should have returned error on malformed input"
    exit 1
fi
echo "Passed: Malformed input rejected with non-zero exit code."

echo "=== All CLI integration tests PASSED successfully ==="
