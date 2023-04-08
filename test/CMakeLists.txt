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
include(GoogleTest)

set(TESTS
    dijkstra_test
    graph_test
    node_test
    point_test
)

foreach(T IN LISTS TESTS)
    add_executable(${T} ${T}.cpp)
    target_include_directories(${T} PRIVATE ../src/library)
    target_link_libraries(${T}
        GTest::gtest_main
        dijkstra-lib
    )
    gtest_discover_tests(${T})
endforeach()
