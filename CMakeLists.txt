cmake_minimum_required(VERSION 3.22)

# Set CMP0135 to NEW so FetchContent uses timestamps of extraction
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(Dijkstra
    VERSION 0.2.0
    DESCRIPTION "A modern C++ implementation of Dijkstra's algorithm and graph utilities"
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Interface target for strict compiler warnings on our own code
add_library(dijkstra-compiler-flags INTERFACE)
if(MSVC)
    target_compile_options(dijkstra-compiler-flags INTERFACE /W4 /permissive-)
else()
    target_compile_options(dijkstra-compiler-flags INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
    )
endif()

# Sanitizers option
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(dijkstra-compiler-flags INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(dijkstra-compiler-flags INTERFACE -fsanitize=address,undefined)
endif()

# Option to build GUI
option(BUILD_GUI "Build the graphical interactive UI (Dear ImGui + GLFW)" ON)
if(BUILD_GUI)
    find_package(OpenGL REQUIRED)
endif()

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
