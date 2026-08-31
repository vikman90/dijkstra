add_library(dijkstra-lib STATIC
    dijkstra.cpp
    graph.cpp
    point.cpp
)

set_target_properties(dijkstra-lib
    PROPERTIES OUTPUT_NAME dijkstra
)

target_include_directories(dijkstra-lib
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(dijkstra-lib PUBLIC cxx_std_20)

target_link_libraries(dijkstra-lib PUBLIC
    dijkstra-compiler-flags
)

install(TARGETS dijkstra-lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/dijkstra
    DESTINATION include
)
