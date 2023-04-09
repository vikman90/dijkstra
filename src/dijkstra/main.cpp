/**
 * @file main.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Main function file for the dijstra program
 * @version 0.1
 * @date 2023-03-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <chrono>
#include <dijkstra.h>
#include <fstream>
#include <iostream>

using namespace std;
using namespace std::chrono;

/**
 * @brief Main function of the dijkstra application
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 * @return Program exit code
 */

int main(int argc, char **argv) {
    auto graph = Graph(0);

    if (argc > 1 && string(argv[1]) != "-") {
        ifstream file(argv[1]);

        if (file.fail()) {
            cerr << "ERROR: cannot open file '" << argv[1] << "'.\n";
            return EXIT_FAILURE;
        }

        file >> graph;
    } else {
        cin >> graph;
    }

    cerr << "Running Dijkstra's algorithm... ";

    auto start = high_resolution_clock::now();
    auto nodes = dijkstra(graph, 0);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    cerr << "[" << duration.count() << " ms.]\n";

    if (!printNodes(nodes)) {
        cerr << "WARN: The graph is not connected.\n";
    }

    return EXIT_SUCCESS;
}
