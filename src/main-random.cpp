// April 7, 2023

#include <chrono>
#include <iostream>
#include "graph.hpp"

using namespace std;
using namespace std::chrono;

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
