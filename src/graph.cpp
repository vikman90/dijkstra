// March 18, 2023

#include <climits>
#include <cmath>
#include <iostream>
#include "graph.hpp"
#include "point.hpp"

using namespace std;

Graph::Graph(unsigned long size) : weights(size, vector<double>(size, 0)) { }

unsigned long Graph::size() const {
    return weights.size();
}

double Graph::getWeight(unsigned long i, unsigned long j) const {
    return weights[i][j];
}

void Graph::setWeight(unsigned long i, unsigned long j, double value) {
    weights[i][j] = weights[j][i] = value;
}

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
