# April 2, 2023

cmake_minimum_required(VERSION 3.22)

project(Dijkstra
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_executable(dijkstra
    src/dijkstra.cpp
    src/graph.cpp
    src/main-dijkstra.cpp
    src/node.cpp
    src/point.cpp
)

add_executable(random-graph
    src/graph.cpp
    src/main-random.cpp
    src/point.cpp
)

install(TARGETS dijkstra random-graph
    DESTINATION bin
)
