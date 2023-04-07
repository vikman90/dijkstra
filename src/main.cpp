// March 18, 2023

#include <chrono>
#include <iostream>
#include "dijkstra.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char ** argv) {
    auto size = argc > 1 ? strtoul(argv[1], nullptr, 10) : 100;
    auto connections = argc > 2 ? strtoul(argv[2], nullptr, 10) : 3;

    cerr << "Generating random graph...    ";

    auto start = high_resolution_clock::now();
    auto graph = Graph::random(size, connections);
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    cerr << "[" << duration.count() << " ms.]\n";

    cerr << "Running Dijkstra's algorithm... ";

    start = high_resolution_clock::now();
    auto nodes = dijkstra(graph, 0);
    stop = high_resolution_clock::now();

    duration = duration_cast<milliseconds>(stop - start);
    cerr << "[" << duration.count() << " ms.]\n";

    if (!printNodes(nodes)) {
        cerr << "WARN: The graph is not connected.\n";
    }

    return EXIT_SUCCESS;
}
