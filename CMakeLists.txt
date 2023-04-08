# April 2, 2023

cmake_minimum_required(VERSION 3.22)

project(Dijkstra
    VERSION 1.0
    DESCRIPTION "A C++ implementation of Dijkstra's algorithm"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_EXTRACT_PRIVATE yes)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    set(DOXYGEN_AUTOLINK_SUPPORT no)

    doxygen_add_docs(docs
        README.md
        src
    )
endif()
