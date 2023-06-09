/**
 * @file dijkstra.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Dijkstra function implementation
 * @version 0.1
 * @date 2023-04-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dijkstra.h"

using std::vector;

// Compute the Dijkstra's algorithm

vector<Node> dijkstra(const Graph &graph, unsigned long begin) {
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
