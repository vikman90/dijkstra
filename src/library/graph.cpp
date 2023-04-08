/**
 * @file graph.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Implementation of class Graph
 * @version 0.1
 * @date 2023-03-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <sstream>
#include "graph.h"
#include "point.h"

using namespace std;

// Get a sample graph

Graph Graph::sample() {
    auto graph = Graph(6);

    graph.setWeight(0, 1, 7);
    graph.setWeight(0, 2, 9);
    graph.setWeight(0, 5, 14);
    graph.setWeight(1, 2, 10);
    graph.setWeight(1, 3, 15);
    graph.setWeight(2, 3, 11);
    graph.setWeight(2, 5, 2);
    graph.setWeight(3, 4, 6);
    graph.setWeight(4, 5, 9);

    return graph;
}

// Create a random graph

Graph Graph::random(unsigned long size, unsigned connections) {
    auto graph = Graph(size);
    auto points = randomPoints(size);

    for (auto i = 0ul; i < size; i++) {
        auto queue = EdgeQueue();

        for (auto j = i + 1; j < size; j++) {
            auto d = sqrt(pow(points[i].x - points[j].x, 2) + pow(points[i].y - points[j].y, 2));
            queue.push(Edge { .p1 = i, .p2 = j, .distance = d });
        }

        for (auto n = 0; n < connections && !queue.empty(); n++) {
            auto edge = queue.top();
            graph.setWeight(edge.p1, edge.p2, edge.distance);
            queue.pop();

        }
    }

    return graph;
}

// Resize a graph

void Graph::resize(unsigned long size) {
    _size = size;
    weights.resize(size * size);
}

/**
 * @brief Insertion operator
 *
 * @param os Output stream
 * @param graph A graph
 * @return A reference to the output stream
 */
std::ostream & operator << (std::ostream & os, const Graph & graph) {
    for (auto i = 0ul; i < graph._size; i++) {
        for (auto j = i + 1; j < graph._size; j++) {
            os << graph.getWeight(i, j) << (j < graph._size - 1 ? ' ' : '\n');
        }
    }

    return os;
}

/**
 * @brief Extraction operator
 *
 * @param is Input stream
 * @param graph A graph
 * @return A reference to the input stream
 */
std::istream & operator >> (std::istream & is, Graph & graph) {
    auto buf = stringbuf();
    is.get(buf, '\n');
    auto str = buf.str();
    auto size = count(str.begin(), str.end(), ' ') + 2;

    graph.resize(size);

    // First line

    {
        auto ss = stringstream(str);

        for (auto j = 1ul; j < size; j++) {
            double weight;
            ss >> weight;
            graph.setWeight(0, j, weight);
        }
    }

    // Rest of lines

    for (auto i = 1ul; i < size; i++) {
        for (auto j = i + 1; j < size; j++) {
            double weight;
            is >> weight;
            graph.setWeight(i, j, weight);
        }
    }

    if (is.fail()) {
        cerr << "ERROR: cannot load data. Check file format.\n";
        exit(EXIT_FAILURE);
    }

    return is;
}
