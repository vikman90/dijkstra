// March 18, 2023

#include <chrono>
#include <fstream>
#include <iostream>
#include "dijkstra.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv) {
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
