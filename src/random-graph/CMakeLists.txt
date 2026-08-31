add_executable(random-graph
    main.cpp
)

target_link_libraries(random-graph
    PRIVATE
        dijkstra-lib
        dijkstra-compiler-flags
)

install(TARGETS random-graph
    RUNTIME DESTINATION bin
)
