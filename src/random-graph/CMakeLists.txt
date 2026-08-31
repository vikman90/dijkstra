add_executable(random-graph
    main.cpp
)

target_link_libraries(random-graph
    PRIVATE
        dijkstra-lib
)

install(TARGETS random-graph
    RUNTIME DESTINATION bin
)
