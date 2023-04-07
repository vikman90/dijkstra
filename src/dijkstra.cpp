// April 1, 2023

#include "dijkstra.hpp"

using std::vector;

static vector<Node> createNodes(unsigned long size) {
    auto nodes = vector<Node>(size);

    for (auto i = 0ul; i < size; i++) {
        nodes[i].index = i;
    }

    return nodes;
}

vector<Node> dijkstra(const Graph & graph, unsigned long begin) {
    auto nodes = createNodes(graph.size);
    auto queue = NodeQueue();

    nodes[begin].cost = 0;
    queue.push(&nodes[begin]);

    while (!queue.empty()) {
        auto cur_node = queue.top();
        queue.pop();

        if (cur_node->visited) {
            continue;
        }

        cur_node->visited = true;

        for (auto i = 0ul; i < graph.size; i++) {
            if (nodes[i].visited) {
                continue;
            }

            auto weight = graph.getWeight(cur_node->index, i);

            if (weight == 0) {
                continue;
            }

            auto cost = nodes[cur_node->index].cost + weight;

            if (cost < nodes[i].cost) {
                nodes[i].prev = cur_node->index;
                nodes[i].cost = cost;
                queue.push(&nodes[i]);
            }
        }
    }

    return nodes;
}
