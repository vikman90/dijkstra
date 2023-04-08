add_executable(random-graph
    main.cpp
)

target_link_libraries(random-graph
    dijkstra-lib
)

target_include_directories(random-graph
    PRIVATE ../library
)

install(TARGETS random-graph
    DESTINATION bin
)
