add_subdirectory(library)
add_subdirectory(dijkstra)
add_subdirectory(random-graph)

if(BUILD_GUI)
    add_subdirectory(gui)
endif()
