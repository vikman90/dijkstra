# April 2, 2023

cmake_minimum_required(VERSION 3.22)

project(Dijkstra
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(dijkstra
    src/dijkstra.cpp
    src/graph.cpp
    src/main.cpp
    src/node.cpp
    src/point.cpp
)

install(TARGETS dijkstra
    DESTINATION bin
)
