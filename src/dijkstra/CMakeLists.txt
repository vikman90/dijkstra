add_executable(dijkstra-bin
    main.cpp
)

target_link_libraries(dijkstra-bin
    PRIVATE
        dijkstra-lib
)

set_target_properties(dijkstra-bin
    PROPERTIES OUTPUT_NAME dijkstra
)

install(TARGETS dijkstra-bin
    RUNTIME DESTINATION bin
)
