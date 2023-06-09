# Dijkstra

[![CMake](https://github.com/vikman90/dijkstra/actions/workflows/cmake.yml/badge.svg)](https://github.com/vikman90/dijkstra/actions/workflows/cmake.yml)

C++ implementation of [Dijkstra's algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)

## Documentation

https://vikman90.github.io/dijkstra

## Setup

### Requirements

- C++ compiler (clang preferred).
- cmake 3.22 or above.

#### Setup on Ubuntu 22.04

```shell
sudo apt install clang cmake
```

### Build

```shell
cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=no
cmake --build build -j$(nproc)
```

### Install

```shell
sudo cmake --install build
```

### Uninstall

```shell
sudo xargs rm -v < build/install_manifest.txt
```

## Run

### Random graph

```
random-graph [ SIZE [ CONNECTIONS ] ]
```

|Option|Description|Range|Default|
|---|---|---|---|
|`SIZE`|Number of nodes.|0 < `SIZE` < 4294967295|100|
|`CONNECTIONS`|Number of connections per node.|0 < `CONNECTIONS` < 4294967295|3|

### Dijkstra

```
dijkstra [ FILE ]
```

|Option|Description|Range|Default|
|---|---|---|---|
|`FILE`|Graph file.|File path|_stdin_|

### Example

```shell
random-graph 5 | dijkstra
```

> ```
> Generating a random graph...    [0 ms.]
> Running Dijkstra's algorithm... [0 ms.]
>   → 0 [0]
> 2 → 1 [2.14492]
> 0 → 2 [0.671249]
> 0 → 3 [0.333003]
> 0 → 4 [1.7423]
> ```

## Advanced

### Run tests

```shell
cmake -B build -DCMAKE_CXX_COMPILER=clang++
cmake --build build -j$(nproc)
ctest --test-dir build
```

### Generate UML diagrams

```shell
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build
clang-uml
```

#### Requisites

- [clang-uml](https://github.com/bkryza/clang-uml)

##### Setup on Ubuntu 22.04

```shell
sudo add-apt-repository ppa:bkryza/clang-uml
sudo apt install clang-uml
```

### Generate Doxygen documentation

```shell
cmake -B build
cmake --build build --target docs
```

#### Requisites

- Doxygen
- Graphviz

##### Setup on Ubuntu 22.04

```shell
sudo apt install doxygen graphviz
```
