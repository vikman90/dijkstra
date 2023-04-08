add_library(dijkstra-lib STATIC
    dijkstra.cpp
    graph.cpp
    node.cpp
    point.cpp
)

set_target_properties(dijkstra-lib
    PROPERTIES OUTPUT_NAME dijkstra
)
