# Dijkstra
C++ implementation of [Dijkstra's algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)

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
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -B build
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

### Usage

```
dijkstra [ SIZE [ CONNECTIONS ] ]
```

|Option|Description|Range|Default|
|---|---|---|---|
|`SIZE`|Number of nodes.|0 < `SIZE` < 4294967295|100|
|`CONNECTIONS`|Number of connections per node.|0 < `CONNECTIONS` < 4294967295|3|

### Example

```shell
dijkstra 5
```

> ```
> Generating random graph...    [0 ms.]
> Running Dijkstra algorithm... [0 ms.]
>   → 0 [0]
> 2 → 1 [2.14492]
> 0 → 2 [0.671249]
> 0 → 3 [0.333003]
> 0 → 4 [1.7423]
> ```
