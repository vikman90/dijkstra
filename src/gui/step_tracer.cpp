/**
 * @file step_tracer.cpp
 * @brief Implementation of StepTracer.
 */

#include "step_tracer.h"

#include <algorithm>
#include <iomanip>
#include <queue>
#include <sstream>

namespace dijkstra::gui {

namespace {

std::vector<NodeId> reconstruct_path(
    NodeId source,
    NodeId destination,
    const std::vector<NodeId> &predecessors
) {
    std::vector<NodeId> path;
    if (destination >= predecessors.size()) {
        return path;
    }
    for (NodeId curr = destination; curr != kNullNode; curr = predecessors[curr]) {
        path.push_back(curr);
        if (curr == source) {
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    if (path.empty() || path.front() != source) {
        return {};
    }
    return path;
}

std::string format_weight(Weight w) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << w;
    std::string s = oss.str();
    if (s.size() >= 2 && s.substr(s.size() - 2) == ".0") {
        s.erase(s.size() - 2);
    }
    return s;
}

} // namespace

std::vector<DijkstraStep> StepTracer::trace(
    const Graph &graph,
    NodeId source,
    std::optional<NodeId> target
) {
    const std::size_t n = graph.node_count();
    if (source >= n || (target.has_value() && *target >= n)) {
        return {};
    }

    std::vector<DijkstraStep> steps;

    std::vector<Weight> dist(n, kInfinity);
    std::vector<NodeId> prev(n, kNullNode);
    std::vector<NodeVisualState> states(n, NodeVisualState::UNVISITED);

    using QueueElement = std::pair<Weight, NodeId>;
    std::priority_queue<
        QueueElement,
        std::vector<QueueElement>,
        std::greater<QueueElement>>
        pq;

    dist[source] = 0.0;
    states[source] = NodeVisualState::OPEN;
    pq.emplace(0.0, source);

    std::size_t relax_count = 0;
    std::size_t settled_count = 0;

    auto make_snapshot = [&](
        StepType type,
        NodeId curr,
        NodeId neigh,
        Weight weight,
        const std::string &desc,
        const std::vector<NodeId> &path = {}
    ) {
        // Collect current priority queue elements
        std::vector<std::pair<Weight, NodeId>> pq_items;
        auto pq_copy = pq;
        while (!pq_copy.empty()) {
            pq_items.push_back(pq_copy.top());
            pq_copy.pop();
        }

        DijkstraStep s;
        s.step_index = steps.size();
        s.type = type;
        s.current_node = curr;
        s.neighbor_node = neigh;
        s.edge_weight = weight;
        s.node_states = states;
        s.distances = dist;
        s.predecessors = prev;
        s.priority_queue_snapshot = std::move(pq_items);
        s.active_path = path;
        s.description = desc;
        s.relaxations_count = relax_count;
        s.settled_count = settled_count;
        steps.push_back(std::move(s));
    };

    // Step 0: Initial state
    make_snapshot(
        StepType::START,
        source,
        kNullNode,
        0.0,
        "Initialized search from source vertex " + std::to_string(source) + " (distance = 0)."
    );

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // If already settled, skip stale entry
        if (states[u] == NodeVisualState::SETTLED) {
            continue;
        }

        states[u] = NodeVisualState::SETTLED;
        ++settled_count;

        make_snapshot(
            StepType::EXTRACT_MIN,
            u,
            kNullNode,
            d,
            "Extracted vertex " + std::to_string(u) + " with minimum tentative distance " +
                format_weight(d) + ". Vertex settled."
        );

        if (target.has_value() && u == *target) {
            auto path = reconstruct_path(source, *target, prev);
            for (NodeId p : path) {
                states[p] = NodeVisualState::IN_PATH;
            }
            make_snapshot(
                StepType::TARGET_REACHED,
                u,
                kNullNode,
                d,
                "Target vertex " + std::to_string(*target) + " reached! Optimal path distance: " +
                    format_weight(d) + ".",
                path
            );
            return steps;
        }

        for (const auto &edge : graph.neighbors(u)) {
            NodeId v = edge.to;
            if (states[v] == NodeVisualState::SETTLED) {
                continue;
            }

            Weight new_cost = dist[u] + edge.weight;
            if (new_cost < dist[v]) {
                dist[v] = new_cost;
                prev[v] = u;
                if (states[v] != NodeVisualState::SETTLED) {
                    states[v] = NodeVisualState::OPEN;
                }
                pq.emplace(new_cost, v);
                ++relax_count;

                make_snapshot(
                    StepType::RELAX_SUCCESS,
                    u,
                    v,
                    edge.weight,
                    "Relaxed edge (" + std::to_string(u) + " -> " + std::to_string(v) + "): improved distance to " +
                        format_weight(new_cost) + "."
                );
            } else {
                make_snapshot(
                    StepType::RELAX_SKIP,
                    u,
                    v,
                    edge.weight,
                    "Checked edge (" + std::to_string(u) + " -> " + std::to_string(v) + "): current distance (" +
                        format_weight(dist[v]) + ") <= proposed (" + format_weight(new_cost) + "). No update."
                );
            }
        }
    }

    // Final completion step
    std::vector<NodeId> final_path;
    if (target.has_value() && dist[*target] < kInfinity) {
        final_path = reconstruct_path(source, *target, prev);
        for (NodeId p : final_path) {
            states[p] = NodeVisualState::IN_PATH;
        }
    }

    make_snapshot(
        StepType::COMPLETE,
        kNullNode,
        kNullNode,
        0.0,
        "Dijkstra search complete. All reachable vertices settled.",
        final_path
    );

    return steps;
}

} // namespace dijkstra::gui
