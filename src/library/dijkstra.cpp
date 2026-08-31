/**
 * @file dijkstra.cpp
 * @brief Implementation of Dijkstra's shortest path algorithm and DijkstraResult methods.
 */

#include "dijkstra/dijkstra.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <utility>

namespace dijkstra {

DijkstraResult::DijkstraResult(
    NodeId source,
    std::optional<NodeId> target,
    std::vector<Weight> distances,
    std::vector<NodeId> predecessors,
    std::size_t visited_count
)
    : source_(source),
      target_(target),
      distances_(std::move(distances)),
      predecessors_(std::move(predecessors)),
      visited_count_(visited_count) {}

bool DijkstraResult::has_path_to(NodeId destination) const {
    if (destination >= distances_.size()) {
        return false;
    }
    return distances_[destination] < kInfinity;
}

std::optional<Weight> DijkstraResult::distance_to(NodeId destination) const {
    if (!has_path_to(destination)) {
        return std::nullopt;
    }
    return distances_[destination];
}

std::optional<std::vector<NodeId>> DijkstraResult::path_to(NodeId destination) const {
    if (!has_path_to(destination)) {
        return std::nullopt;
    }

    std::vector<NodeId> path;
    for (NodeId curr = destination; curr != kNullNode; curr = predecessors_[curr]) {
        path.push_back(curr);
        if (curr == source_) {
            break;
        }
    }

    std::reverse(path.begin(), path.end());
    if (path.empty() || path.front() != source_) {
        return std::nullopt;
    }

    return path;
}

bool DijkstraResult::is_connected() const noexcept {
    for (const auto &d : distances_) {
        if (d == kInfinity) {
            return false;
        }
    }
    return true;
}

void DijkstraResult::print_summary(std::ostream &os) const {
    for (NodeId i = 0; i < distances_.size(); ++i) {
        if (i == source_) {
            os << "  → " << i << " [0]\n";
        } else if (distances_[i] == kInfinity || predecessors_[i] == kNullNode) {
            os << "  → " << i << " [X]\n";
        } else {
            os << predecessors_[i] << " → " << i << " [" << distances_[i] << "]\n";
        }
    }
}

namespace {

DijkstraResult run_dijkstra(
    const Graph &graph,
    NodeId source,
    std::optional<NodeId> target
) {
    const std::size_t n = graph.node_count();
    if (source >= n) {
        throw InvalidNodeException("Source node index out of bounds");
    }
    if (target.has_value() && *target >= n) {
        throw InvalidNodeException("Target node index out of bounds");
    }

    std::vector<Weight> dist(n, kInfinity);
    std::vector<NodeId> prev(n, kNullNode);
    std::vector<bool> visited(n, false);
    std::size_t visited_count = 0;

    using QueueElement = std::pair<Weight, NodeId>;
    std::priority_queue<
        QueueElement,
        std::vector<QueueElement>,
        std::greater<QueueElement>>
        pq;

    dist[source] = 0.0;
    pq.emplace(0.0, source);

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (visited[u]) {
            continue;
        }

        visited[u] = true;
        ++visited_count;

        if (target.has_value() && u == *target) {
            break;
        }

        for (const auto &edge : graph.neighbors(u)) {
            NodeId v = edge.to;
            if (visited[v]) {
                continue;
            }

            Weight new_cost = dist[u] + edge.weight;
            if (new_cost < dist[v]) {
                dist[v] = new_cost;
                prev[v] = u;
                pq.emplace(new_cost, v);
            }
        }
    }

    return DijkstraResult(source, target, std::move(dist), std::move(prev), visited_count);
}

} // namespace

DijkstraResult shortest_paths(const Graph &graph, NodeId source) {
    return run_dijkstra(graph, source, std::nullopt);
}

DijkstraResult shortest_path(const Graph &graph, NodeId source, NodeId target) {
    return run_dijkstra(graph, source, target);
}

} // namespace dijkstra
