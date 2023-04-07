/**
 * @file main-random.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Main function file for the random-graph program
 * @version 0.1
 * @date 2023-04-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <chrono>
#include <iostream>
#include "graph.hpp"

using namespace std;
using namespace std::chrono;

/**
 * @brief Main function of the random-graph application
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 * @return Program exit code
 */

int main(int argc, char ** argv) {
    auto size = argc > 1 ? strtoul(argv[1], nullptr, 10) : 100;
    auto connections = argc > 2 ? strtoul(argv[2], nullptr, 10) : 3;

    cerr << "Generating a random graph...    ";

    auto start = high_resolution_clock::now();
    auto graph = Graph::random(size, connections);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    cerr << "[" << duration.count() << " ms.]\n";

    cout << graph;

    return EXIT_SUCCESS;
}
