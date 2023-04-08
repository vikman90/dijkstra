include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(INSTALL_GTEST off)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(dijkstra_test dijkstra_test.cpp)
target_include_directories(dijkstra_test PRIVATE ../src/library)
target_link_libraries(dijkstra_test
    GTest::gtest_main
    dijkstra-lib
)

add_executable(graph_test graph_test.cpp)
target_include_directories(graph_test PRIVATE ../src/library)
target_link_libraries(graph_test
    GTest::gtest_main
    dijkstra-lib
)

include(GoogleTest)

gtest_discover_tests(dijkstra_test)
gtest_discover_tests(graph_test)
