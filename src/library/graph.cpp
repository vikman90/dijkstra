/**
 * @file graph.cpp
 * @brief Implementation of the Graph class, serialization, and generation algorithms.
 */

#include "dijkstra/graph.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>

namespace dijkstra {

Graph::Graph(std::size_t node_count, bool directed)
    : nodes_(node_count), edge_count_(0), directed_(directed), adj_(node_count) {}

void Graph::add_edge(NodeId u, NodeId v, Weight weight) {
    if (u >= nodes_ || v >= nodes_) {
        throw InvalidNodeException("Node index out of bounds in add_edge");
    }
    if (weight < 0.0) {
        throw NegativeWeightException("Edge weight cannot be negative");
    }

    if (weight == 0.0) {
        // Remove edge if it exists
        auto it = std::remove_if(adj_[u].begin(), adj_[u].end(), [v](const Edge &e) {
            return e.to == v;
        });
        if (it != adj_[u].end()) {
            adj_[u].erase(it, adj_[u].end());
            if (edge_count_ > 0) {
                --edge_count_;
            }
        }
        if (!directed_ && u != v) {
            auto it2 = std::remove_if(adj_[v].begin(), adj_[v].end(), [u](const Edge &e) {
                return e.to == u;
            });
            if (it2 != adj_[v].end()) {
                adj_[v].erase(it2, adj_[v].end());
            }
        }
        return;
    }

    // Update existing edge or insert new
    bool found = false;
    for (auto &edge : adj_[u]) {
        if (edge.to == v) {
            edge.weight = weight;
            found = true;
            break;
        }
    }
    if (!found) {
        adj_[u].push_back(Edge{.from = u, .to = v, .weight = weight});
        ++edge_count_;
    }

    if (!directed_ && u != v) {
        bool rev_found = false;
        for (auto &edge : adj_[v]) {
            if (edge.to == u) {
                edge.weight = weight;
                rev_found = true;
                break;
            }
        }
        if (!rev_found) {
            adj_[v].push_back(Edge{.from = v, .to = u, .weight = weight});
        }
    }
}

bool Graph::has_edge(NodeId u, NodeId v) const {
    if (u >= nodes_ || v >= nodes_) {
        return false;
    }
    for (const auto &edge : adj_[u]) {
        if (edge.to == v && edge.weight > 0.0) {
            return true;
        }
    }
    return false;
}

Weight Graph::get_weight(NodeId u, NodeId v) const {
    if (u >= nodes_ || v >= nodes_) {
        throw InvalidNodeException("Node index out of bounds in get_weight");
    }
    if (u == v) {
        return 0.0;
    }
    for (const auto &edge : adj_[u]) {
        if (edge.to == v) {
            return edge.weight;
        }
    }
    return 0.0;
}

const std::vector<Edge> &Graph::neighbors(NodeId u) const {
    if (u >= nodes_) {
        throw InvalidNodeException("Node index out of bounds in neighbors");
    }
    return adj_[u];
}

std::vector<Edge> Graph::all_edges() const {
    std::vector<Edge> edges;
    edges.reserve(edge_count_);
    for (NodeId u = 0; u < nodes_; ++u) {
        for (const auto &edge : adj_[u]) {
            if (directed_ || edge.from <= edge.to) {
                edges.push_back(edge);
            }
        }
    }
    return edges;
}

void Graph::resize(std::size_t new_node_count) {
    nodes_ = new_node_count;
    adj_.resize(new_node_count);
    // Filter out edges pointing to nodes beyond new_node_count
    edge_count_ = 0;
    for (NodeId u = 0; u < nodes_; ++u) {
        adj_[u].erase(
            std::remove_if(adj_[u].begin(), adj_[u].end(), [this](const Edge &e) {
                return e.to >= nodes_;
            }),
            adj_[u].end()
        );
        if (directed_) {
            edge_count_ += adj_[u].size();
        } else {
            for (const auto &e : adj_[u]) {
                if (e.from <= e.to) {
                    ++edge_count_;
                }
            }
        }
    }
}

void Graph::clear() noexcept {
    nodes_ = 0;
    edge_count_ = 0;
    adj_.clear();
}

std::string Graph::to_dot(const std::string &graph_name) const {
    std::ostringstream oss;
    if (directed_) {
        oss << "digraph " << graph_name << " {\n";
        oss << "  rankdir=LR;\n";
        oss << "  node [shape=circle];\n";
        for (NodeId u = 0; u < nodes_; ++u) {
            for (const auto &edge : adj_[u]) {
                oss << "  " << edge.from << " -> " << edge.to
                    << " [label=\"" << edge.weight << "\"];\n";
            }
        }
    } else {
        oss << "graph " << graph_name << " {\n";
        oss << "  rankdir=LR;\n";
        oss << "  node [shape=circle];\n";
        for (const auto &edge : all_edges()) {
            oss << "  " << edge.from << " -- " << edge.to
                << " [label=\"" << edge.weight << "\"];\n";
        }
    }
    oss << "}\n";
    return oss.str();
}

bool Graph::operator==(const Graph &other) const {
    if (nodes_ != other.nodes_ || directed_ != other.directed_) {
        return false;
    }
    for (NodeId i = 0; i < nodes_; ++i) {
        for (NodeId j = 0; j < nodes_; ++j) {
            if (std::abs(get_weight(i, j) - other.get_weight(i, j)) > 1e-9) {
                return false;
            }
        }
    }
    return true;
}

bool Graph::operator!=(const Graph &other) const {
    return !(*this == other);
}

Graph Graph::sample() {
    Graph graph(6, false);
    graph.add_edge(0, 1, 7.0);
    graph.add_edge(0, 2, 9.0);
    graph.add_edge(0, 5, 14.0);
    graph.add_edge(1, 2, 10.0);
    graph.add_edge(1, 3, 15.0);
    graph.add_edge(2, 3, 11.0);
    graph.add_edge(2, 5, 2.0);
    graph.add_edge(3, 4, 6.0);
    graph.add_edge(4, 5, 9.0);
    return graph;
}

bool Graph::is_connected() const {
    if (nodes_ <= 1) {
        return true;
    }
    std::vector<bool> visited(nodes_, false);
    std::queue<NodeId> q;
    visited[0] = true;
    q.push(0);
    std::size_t visited_count = 1;

    while (!q.empty()) {
        NodeId u = q.front();
        q.pop();
        for (const auto &edge : adj_[u]) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                ++visited_count;
                q.push(edge.to);
            }
        }
    }
    return visited_count == nodes_;
}

Graph Graph::random_geometric(
    std::size_t nodes,
    std::size_t connections,
    std::optional<std::uint64_t> seed
) {
    Graph graph(nodes, false);
    if (nodes <= 1) {
        return graph;
    }

    auto points = random_points(nodes, seed);

    // 1. Guarantee connectivity using Euclidean Minimum Spanning Tree (MST via Prim's algorithm)
    std::vector<bool> in_mst(nodes, false);
    std::vector<double> min_dist(nodes, kInfinity);
    std::vector<NodeId> parent(nodes, kNullNode);

    min_dist[0] = 0.0;

    for (std::size_t step = 0; step < nodes; ++step) {
        NodeId u = kNullNode;
        double best_dist = kInfinity;
        for (NodeId i = 0; i < nodes; ++i) {
            if (!in_mst[i] && min_dist[i] < best_dist) {
                best_dist = min_dist[i];
                u = i;
            }
        }
        if (u == kNullNode) {
            break;
        }

        in_mst[u] = true;
        if (parent[u] != kNullNode) {
            graph.add_edge(parent[u], u, best_dist);
        }

        for (NodeId v = 0; v < nodes; ++v) {
            if (!in_mst[v]) {
                double d = euclidean_distance(points[u], points[v]);
                if (d < min_dist[v]) {
                    min_dist[v] = d;
                    parent[v] = u;
                }
            }
        }
    }

    // 2. Add remaining nearest neighbors up to 'connections' per node
    struct DistanceEdge {
        NodeId p1;
        NodeId p2;
        double dist;
        bool operator>(const DistanceEdge &other) const {
            return dist > other.dist;
        }
    };

    for (NodeId i = 0; i < nodes; ++i) {
        std::priority_queue<
            DistanceEdge,
            std::vector<DistanceEdge>,
            std::greater<DistanceEdge>>
            pq;

        for (NodeId j = 0; j < nodes; ++j) {
            if (i == j) continue;
            double d = euclidean_distance(points[i], points[j]);
            pq.push(DistanceEdge{.p1 = i, .p2 = j, .dist = d});
        }

        std::size_t added = graph.neighbors(i).size();
        while (added < connections && !pq.empty()) {
            const auto top = pq.top();
            pq.pop();
            if (!graph.has_edge(top.p1, top.p2)) {
                graph.add_edge(top.p1, top.p2, top.dist);
                ++added;
            }
        }
    }

    return graph;
}

Graph Graph::random_erdos_renyi(
    std::size_t nodes,
    double edge_probability,
    Weight min_weight,
    Weight max_weight,
    std::optional<std::uint64_t> seed
) {
    Graph graph(nodes, false);
    if (nodes == 0) {
        return graph;
    }

    std::mt19937_64 engine;
    if (seed.has_value()) {
        engine.seed(*seed);
    } else {
        std::random_device rd;
        engine.seed(rd());
    }

    std::bernoulli_distribution has_edge_dist(std::clamp(edge_probability, 0.0, 1.0));
    std::uniform_real_distribution<double> weight_dist(min_weight, max_weight);

    for (NodeId i = 0; i < nodes; ++i) {
        for (NodeId j = i + 1; j < nodes; ++j) {
            if (has_edge_dist(engine)) {
                graph.add_edge(i, j, weight_dist(engine));
            }
        }
    }

    return graph;
}

std::ostream &operator<<(std::ostream &os, const Graph &graph) {
    for (NodeId i = 0; i < graph.nodes_; ++i) {
        for (NodeId j = i + 1; j < graph.nodes_; ++j) {
            os << graph.get_weight(i, j) << (j < graph.nodes_ - 1 ? ' ' : '\n');
        }
    }
    return os;
}

std::istream &operator>>(std::istream &is, Graph &graph) {
    std::string line;
    // Skip empty lines at the beginning
    while (std::getline(is, line)) {
        if (!line.empty() && line.find_first_not_of(" \t\r\n") != std::string::npos) {
            break;
        }
    }

    if (line.empty()) {
        return is;
    }

    std::vector<double> first_line_values;
    {
        std::istringstream ss(line);
        double val;
        while (ss >> val) {
            first_line_values.push_back(val);
        }
    }

    if (first_line_values.empty()) {
        throw GraphParseException("Invalid graph format: empty first line");
    }

    // Number of elements on first line of upper-triangular matrix is size - 1
    std::size_t size = first_line_values.size() + 1;
    graph.clear();
    graph.resize(size);

    for (std::size_t j = 1; j < size; ++j) {
        if (first_line_values[j - 1] > 0.0) {
            graph.add_edge(0, j, first_line_values[j - 1]);
        }
    }

    for (std::size_t i = 1; i < size - 1; ++i) {
        for (std::size_t j = i + 1; j < size; ++j) {
            double weight;
            if (!(is >> weight)) {
                throw GraphParseException("Unexpected end of stream or malformed numeric weight");
            }
            if (weight > 0.0) {
                graph.add_edge(i, j, weight);
            }
        }
    }

    return is;
}

} // namespace dijkstra
