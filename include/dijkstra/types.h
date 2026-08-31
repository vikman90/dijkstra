/**
 * @file types.h
 * @brief Common type definitions and constants for the Dijkstra library.
 */

#pragma once

#include <compare>
#include <cstddef>
#include <limits>
#include <ostream>

namespace dijkstra {

/// @brief Unique identifier for a graph node (0-indexed).
using NodeId = std::size_t;

/// @brief Numeric weight of an edge or accumulated path cost.
using Weight = double;

/// @brief Representation of infinity for unreachable distances.
inline constexpr Weight kInfinity = std::numeric_limits<Weight>::infinity();

/// @brief Sentinel value indicating no predecessor or invalid node.
inline constexpr NodeId kNullNode = std::numeric_limits<NodeId>::max();

/**
 * @brief A directed or undirected weighted edge between two nodes.
 */
struct Edge {
    NodeId from{0};
    NodeId to{0};
    Weight weight{0.0};

    [[nodiscard]] auto operator<=>(const Edge &) const = default;
};

/**
 * @brief Stream output operator for Edge.
 */
inline std::ostream &operator<<(std::ostream &os, const Edge &edge) {
    return os << edge.from << " -> " << edge.to << " [" << edge.weight << "]";
}

} // namespace dijkstra
