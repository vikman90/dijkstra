include(FetchContent)

# Modern GoogleTest release
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

set(UNIT_TESTS
    point_test
    graph_test
    dijkstra_test
    io_test
)

foreach(T IN LISTS UNIT_TESTS)
    add_executable(${T} unit/${T}.cpp)
    target_link_libraries(${T}
        PRIVATE
            GTest::gtest_main
            dijkstra-lib
    )
    target_compile_features(${T} PRIVATE cxx_std_20)
    gtest_discover_tests(${T})
endforeach()
