/**
 * @file graph.h
 * @brief Graph data structure supporting adjacency lists, serialization, and generators.
 */

#pragma once

#include "dijkstra/exceptions.h"
#include "dijkstra/point.h"
#include "dijkstra/types.h"

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace dijkstra {

/**
 * @brief Represents a weighted graph using an adjacency list representation.
 */
class Graph {
  public:
    /**
     * @brief Construct a new Graph.
     *
     * @param node_count Initial number of vertices in the graph.
     * @param directed If true, edges are directed; otherwise edges are bidirectional.
     */
    explicit Graph(std::size_t node_count = 0, bool directed = false);

    /**
     * @brief Get the number of nodes in the graph.
     */
    [[nodiscard]] std::size_t node_count() const noexcept {
        return nodes_;
    }

    /**
     * @brief Get the total number of unique edges in the graph.
     */
    [[nodiscard]] std::size_t edge_count() const noexcept {
        return edge_count_;
    }

    /**
     * @brief Check whether the graph is directed.
     */
    [[nodiscard]] bool is_directed() const noexcept {
        return directed_;
    }

    /**
     * @brief Add or update a weighted edge between node @p u and node @p v.
     *
     * @param u Source vertex index.
     * @param v Destination vertex index.
     * @param weight Non-negative edge weight.
     * @throws InvalidNodeException If u or v is out of bounds.
     * @throws NegativeWeightException If weight is negative.
     */
    void add_edge(NodeId u, NodeId v, Weight weight);

    /**
     * @brief Check if an edge exists between node @p u and node @p v.
     */
    [[nodiscard]] bool has_edge(NodeId u, NodeId v) const;

    /**
     * @brief Get the weight of the edge between node @p u and node @p v.
     *
     * @return Edge weight if connected, or 0.0 if not connected.
     * @throws InvalidNodeException If u or v is out of bounds.
     */
    [[nodiscard]] Weight get_weight(NodeId u, NodeId v) const;

    /**
     * @brief Set the weight of an edge (compatibility helper for add_edge).
     */
    void set_weight(NodeId u, NodeId v, Weight weight) {
        add_edge(u, v, weight);
    }

    /**
     * @brief Get the outgoing edges incident to vertex @p u.
     *
     * @param u Vertex index.
     * @return Constant reference to vector of outgoing edges.
     * @throws InvalidNodeException If u is out of bounds.
     */
    [[nodiscard]] const std::vector<Edge> &neighbors(NodeId u) const;

    /**
     * @brief Retrieve a flat list of all edges in the graph.
     */
    [[nodiscard]] std::vector<Edge> all_edges() const;

    /**
     * @brief Resize the graph to accommodate a new number of nodes.
     *
     * @param new_node_count New node count.
     */
    void resize(std::size_t new_node_count);

    /**
     * @brief Remove all nodes and edges from the graph.
     */
    void clear() noexcept;

    /**
     * @brief Export graph topology to Graphviz DOT format.
     *
     * @param graph_name Name of the graph in DOT header.
     * @return DOT formatted string.
     */
    [[nodiscard]] std::string to_dot(const std::string &graph_name = "G") const;

    /**
     * @brief Equality comparison operator.
     */
    [[nodiscard]] bool operator==(const Graph &other) const;

    /**
     * @brief Inequality comparison operator.
     */
    [[nodiscard]] bool operator!=(const Graph &other) const;

    /**
     * @brief Generate the canonical 6-node Wikipedia sample graph.
     */
    [[nodiscard]] static Graph sample();

    /**
     * @brief Generate a geometric random graph based on 2D Euclidean distance.
     *
     * @param nodes Total number of vertices.
     * @param connections Desired number of closest connections per vertex.
     * @param seed Optional random seed.
     */
    [[nodiscard]] static Graph random_geometric(
        std::size_t nodes,
        std::size_t connections,
        std::optional<std::uint64_t> seed = std::nullopt
    );

    /**
     * @brief Generate an Erdős-Rényi random graph $G(n, p)$.
     *
     * @param nodes Total number of vertices.
     * @param edge_probability Probability $p \in [0, 1]$ of an edge between any pair.
     * @param min_weight Minimum weight for generated edges.
     * @param max_weight Maximum weight for generated edges.
     * @param seed Optional random seed.
     */
    [[nodiscard]] static Graph random_erdos_renyi(
        std::size_t nodes,
        double edge_probability,
        Weight min_weight = 1.0,
        Weight max_weight = 10.0,
        std::optional<std::uint64_t> seed = std::nullopt
    );

    friend std::ostream &operator<<(std::ostream &os, const Graph &graph);
    friend std::istream &operator>>(std::istream &is, Graph &graph);

  private:
    std::size_t nodes_{0};
    std::size_t edge_count_{0};
    bool directed_{false};
    std::vector<std::vector<Edge>> adj_;
};

} // namespace dijkstra
