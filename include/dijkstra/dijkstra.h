/**
 * @file dijkstra.h
 * @brief Dijkstra's shortest path algorithm and DijkstraResult container.
 */

#pragma once

#include "dijkstra/exceptions.h"
#include "dijkstra/graph.h"
#include "dijkstra/types.h"

#include <iosfwd>
#include <optional>
#include <vector>

namespace dijkstra {

/**
 * @brief Encapsulates the results of running Dijkstra's shortest path algorithm.
 */
class DijkstraResult {
  public:
    DijkstraResult(
        NodeId source,
        std::optional<NodeId> target,
        std::vector<Weight> distances,
        std::vector<NodeId> predecessors,
        std::size_t visited_count
    );

    /// @brief Starting vertex of the search.
    [[nodiscard]] NodeId source() const noexcept {
        return source_;
    }

    /// @brief Optional target vertex if a single-destination search was requested.
    [[nodiscard]] std::optional<NodeId> target() const noexcept {
        return target_;
    }

    /// @brief Check whether a path exists from source to @p destination.
    [[nodiscard]] bool has_path_to(NodeId destination) const;

    /// @brief Retrieve the minimum distance from source to @p destination.
    [[nodiscard]] std::optional<Weight> distance_to(NodeId destination) const;

    /// @brief Reconstruct the sequence of vertices forming the shortest path to @p destination.
    [[nodiscard]] std::optional<std::vector<NodeId>> path_to(NodeId destination) const;

    /// @brief Reference to all calculated distances.
    [[nodiscard]] const std::vector<Weight> &distances() const noexcept {
        return distances_;
    }

    /// @brief Reference to all vertex predecessors.
    [[nodiscard]] const std::vector<NodeId> &predecessors() const noexcept {
        return predecessors_;
    }

    /// @brief Number of settled/visited vertices during search execution.
    [[nodiscard]] std::size_t visited_count() const noexcept {
        return visited_count_;
    }

    /// @brief Check if all vertices in the graph were reachable from source.
    [[nodiscard]] bool is_connected() const noexcept;

    /// @brief Print a human-readable summary of paths to the provided stream.
    void print_summary(std::ostream &os) const;

  private:
    NodeId source_;
    std::optional<NodeId> target_;
    std::vector<Weight> distances_;
    std::vector<NodeId> predecessors_;
    std::size_t visited_count_;
};

/**
 * @brief Compute the shortest paths from @p source to all reachable vertices in @p graph.
 *
 * @param graph Input weighted graph.
 * @param source Starting vertex index.
 * @return DijkstraResult containing optimal distances and reconstructed paths.
 * @throws InvalidNodeException If source is out of bounds.
 */
[[nodiscard]] DijkstraResult shortest_paths(const Graph &graph, NodeId source);

/**
 * @brief Compute the shortest path from @p source to @p target with early exit.
 *
 * @param graph Input weighted graph.
 * @param source Starting vertex index.
 * @param target Destination vertex index.
 * @return DijkstraResult containing optimal distances and path to target.
 * @throws InvalidNodeException If source or target is out of bounds.
 */
[[nodiscard]] DijkstraResult shortest_path(const Graph &graph, NodeId source, NodeId target);

} // namespace dijkstra
