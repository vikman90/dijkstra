/**
 * @file step_tracer.h
 * @brief Algorithmic execution tracer for step-by-step Dijkstra visual playback.
 */

#pragma once

#include "dijkstra/graph.h"
#include "dijkstra/types.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace dijkstra::gui {

/// @brief Visual lifecycle state of a graph vertex during search.
enum class NodeVisualState {
    UNVISITED, ///< Vertex has not been discovered yet.
    OPEN,      ///< Vertex is currently candidate in the priority queue (Open set).
    SETTLED,   ///< Shortest distance to vertex is finalized (Closed set).
    IN_PATH    ///< Vertex is part of the final optimal shortest path.
};

/// @brief Nature of the algorithm event occurring at a specific step.
enum class StepType {
    START,            ///< Initial state with source distance set to 0.
    EXTRACT_MIN,      ///< Vertex with minimum tentative distance is popped and settled.
    EXAMINE_NEIGHBOR, ///< Inspecting an outgoing edge to a neighbor vertex.
    RELAX_SUCCESS,    ///< Edge relaxation successful: shorter distance found and updated.
    RELAX_SKIP,       ///< Edge relaxation skipped: existing distance is already smaller or equal.
    TARGET_REACHED,   ///< Search terminated early because the target vertex was reached.
    COMPLETE          ///< All reachable vertices have been explored.
};

/**
 * @brief Complete snapshot of the algorithm state at a discrete timeline step.
 */
struct DijkstraStep {
    std::size_t step_index{0};
    StepType type{StepType::START};
    NodeId current_node{kNullNode};
    NodeId neighbor_node{kNullNode};
    Weight edge_weight{0.0};
    std::vector<NodeVisualState> node_states;
    std::vector<Weight> distances;
    std::vector<NodeId> predecessors;
    std::vector<std::pair<Weight, NodeId>> priority_queue_snapshot;
    std::vector<NodeId> active_path;
    std::string description;
    std::size_t relaxations_count{0};
    std::size_t settled_count{0};
};

/**
 * @brief Utility for recording every atomic state transition of Dijkstra's algorithm.
 */
class StepTracer {
  public:
    /**
     * @brief Trace and record the complete step-by-step execution timeline.
     *
     * @param graph Input graph.
     * @param source Starting vertex.
     * @param target Optional destination vertex.
     * @return Sequence of snapshots representing the execution progression.
     */
    [[nodiscard]] static std::vector<DijkstraStep> trace(
        const Graph &graph,
        NodeId source,
        std::optional<NodeId> target = std::nullopt
    );
};

} // namespace dijkstra::gui
