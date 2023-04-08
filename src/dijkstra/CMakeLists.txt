add_executable(dijkstra-bin
    main.cpp
)

target_link_libraries(dijkstra-bin
    dijkstra-lib
)

target_include_directories(dijkstra-bin
    PRIVATE ../library
)

set_target_properties(dijkstra-bin
    PROPERTIES OUTPUT_NAME dijkstra
)

install(TARGETS dijkstra-bin
    DESTINATION bin
)
