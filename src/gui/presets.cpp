/**
 * @file presets.cpp
 * @brief Implementation of Presets and JSON serialization.
 */

#include "presets.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <queue>
#include <random>
#include <sstream>

namespace dijkstra::gui {

namespace {

float euclidean(const Point &p1, const Point &p2) {
    return static_cast<float>(std::hypot(p1.x - p2.x, p1.y - p2.y));
}

} // namespace

VisualGraph Presets::wikipedia_sample() {
    VisualGraph vg;
    // 6 nodes placed in standard balanced layout
    vg.add_node(360.0f, 400.0f); // Node 0 (Leftmost)
    vg.add_node(540.0f, 250.0f); // Node 1 (Top-Left)
    vg.add_node(540.0f, 550.0f); // Node 2 (Bottom-Left)
    vg.add_node(780.0f, 250.0f); // Node 3 (Top-Right)
    vg.add_node(960.0f, 400.0f); // Node 4 (Rightmost)
    vg.add_node(780.0f, 550.0f); // Node 5 (Bottom-Right)

    vg.add_edge(0, 1, 7.0);
    vg.add_edge(0, 2, 9.0);
    vg.add_edge(0, 5, 14.0);
    vg.add_edge(1, 2, 10.0);
    vg.add_edge(1, 3, 15.0);
    vg.add_edge(2, 3, 11.0);
    vg.add_edge(2, 5, 2.0);
    vg.add_edge(3, 4, 6.0);
    vg.add_edge(4, 5, 9.0);

    return vg;
}

VisualGraph Presets::ring_graph(std::size_t nodes, float radius) {
    VisualGraph vg;
    if (nodes == 0) return vg;

    const float center_x = 400.0f;
    const float center_y = 300.0f;

    for (std::size_t i = 0; i < nodes; ++i) {
        double angle = 2.0 * 3.14159265358979323846 * static_cast<double>(i) / static_cast<double>(nodes);
        vg.add_node(
            center_x + radius * static_cast<float>(std::cos(angle)),
            center_y + radius * static_cast<float>(std::sin(angle))
        );
    }

    for (std::size_t i = 0; i < nodes; ++i) {
        std::size_t next = (i + 1) % nodes;
        float d = euclidean(vg.positions()[i], vg.positions()[next]);
        Weight w = std::round(static_cast<double>(d) * 0.1) * 0.1;
        if (w < 1.0) w = 1.0;
        vg.add_edge(i, next, w);
    }

    return vg;
}

VisualGraph Presets::star_graph(std::size_t leaves, float radius) {
    VisualGraph vg;
    const float center_x = 400.0f;
    const float center_y = 300.0f;

    // Node 0 is center hub
    vg.add_node(center_x, center_y);

    for (std::size_t i = 0; i < leaves; ++i) {
        double angle = 2.0 * 3.14159265358979323846 * static_cast<double>(i) / static_cast<double>(leaves);
        NodeId leaf_id = vg.add_node(
            center_x + radius * static_cast<float>(std::cos(angle)),
            center_y + radius * static_cast<float>(std::sin(angle))
        );
        Weight w = 3.0 + static_cast<double>(i % 5) * 2.0;
        vg.add_edge(0, leaf_id, w);
    }

    return vg;
}

VisualGraph Presets::grid_maze(std::size_t rows, std::size_t cols, float spacing) {
    VisualGraph vg;
    const float start_x = 200.0f;
    const float start_y = 150.0f;

    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            vg.add_node(
                start_x + static_cast<float>(c) * spacing,
                start_y + static_cast<float>(r) * spacing
            );
        }
    }

    // Connect horizontal and vertical neighbors with weights
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            NodeId u = r * cols + c;
            if (c + 1 < cols) {
                NodeId right = r * cols + (c + 1);
                Weight w = 2.0 + static_cast<double>((r + c) % 4);
                vg.add_edge(u, right, w);
            }
            if (r + 1 < rows) {
                NodeId down = (r + 1) * cols + c;
                Weight w = 1.5 + static_cast<double>((r * 2 + c) % 5);
                vg.add_edge(u, down, w);
            }
        }
    }

    return vg;
}

VisualGraph Presets::random_geometric(
    std::size_t nodes,
    std::size_t connections,
    float width,
    float height,
    std::optional<std::uint64_t> seed
) {
    VisualGraph vg;
    if (nodes == 0) return vg;
    if (nodes == 1) {
        vg.add_node(width * 0.5f, height * 0.5f);
        return vg;
    }

    std::mt19937_64 engine;
    if (seed.has_value()) {
        engine.seed(*seed);
    } else {
        std::random_device rd;
        engine.seed(rd());
    }

    const float margin_x = 100.0f;
    const float margin_y = 100.0f;
    std::uniform_real_distribution<float> dist_x(margin_x, margin_x + width);
    std::uniform_real_distribution<float> dist_y(margin_y, margin_y + height);

    for (std::size_t i = 0; i < nodes; ++i) {
        vg.add_node(dist_x(engine), dist_y(engine));
    }

    // 1. Guarantee connectivity using Euclidean Minimum Spanning Tree (MST)
    std::vector<bool> in_mst(nodes, false);
    std::vector<float> min_dist(nodes, 1e9f);
    std::vector<NodeId> parent(nodes, kNullNode);

    min_dist[0] = 0.0f;

    for (std::size_t step = 0; step < nodes; ++step) {
        NodeId u = kNullNode;
        float best_dist = 1e9f;
        for (NodeId i = 0; i < nodes; ++i) {
            if (!in_mst[i] && min_dist[i] < best_dist) {
                best_dist = min_dist[i];
                u = i;
            }
        }
        if (u == kNullNode) break;

        in_mst[u] = true;
        if (parent[u] != kNullNode) {
            Weight w = std::round(static_cast<double>(best_dist) * 0.1) * 0.1;
            if (w < 1.0) w = 1.0;
            vg.add_edge(parent[u], u, w);
        }

        for (NodeId v = 0; v < nodes; ++v) {
            if (!in_mst[v]) {
                float d = euclidean(vg.positions()[u], vg.positions()[v]);
                if (d < min_dist[v]) {
                    min_dist[v] = d;
                    parent[v] = u;
                }
            }
        }
    }

    // 2. Add remaining nearest neighbors up to 'connections' per node
    struct DistEdge {
        NodeId p1;
        NodeId p2;
        float d;
        bool operator>(const DistEdge &other) const { return d > other.d; }
    };

    for (NodeId i = 0; i < nodes; ++i) {
        std::priority_queue<DistEdge, std::vector<DistEdge>, std::greater<DistEdge>> pq;
        for (NodeId j = 0; j < nodes; ++j) {
            if (i == j) continue;
            float d = euclidean(vg.positions()[i], vg.positions()[j]);
            pq.push(DistEdge{.p1 = i, .p2 = j, .d = d});
        }
        std::size_t added = vg.graph().neighbors(i).size();
        while (added < connections && !pq.empty()) {
            auto top = pq.top();
            pq.pop();
            if (!vg.graph().has_edge(top.p1, top.p2)) {
                Weight w = std::round(static_cast<double>(top.d) * 0.1) * 0.1;
                if (w < 1.0) w = 1.0;
                vg.add_edge(top.p1, top.p2, w);
                ++added;
            }
        }
    }

    return vg;
}

bool Presets::save_to_json(const VisualGraph &vg, const std::string &filepath, std::string &error_msg) {
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
        error_msg = "Could not open file for writing: " + filepath;
        return false;
    }

    const auto &positions = vg.positions();
    const auto edges = vg.graph().all_edges();

    ofs << "{\n";
    ofs << "  \"nodes\": [\n";
    for (std::size_t i = 0; i < positions.size(); ++i) {
        ofs << "    {\"id\": " << i
            << ", \"x\": " << std::fixed << std::setprecision(1) << positions[i].x
            << ", \"y\": " << std::fixed << std::setprecision(1) << positions[i].y
            << "}" << (i + 1 < positions.size() ? "," : "") << "\n";
    }
    ofs << "  ],\n";
    ofs << "  \"edges\": [\n";
    for (std::size_t i = 0; i < edges.size(); ++i) {
        ofs << "    {\"from\": " << edges[i].from
            << ", \"to\": " << edges[i].to
            << ", \"weight\": " << std::fixed << std::setprecision(2) << edges[i].weight
            << "}" << (i + 1 < edges.size() ? "," : "") << "\n";
    }
    ofs << "  ]\n";
    ofs << "}\n";

    return true;
}

bool Presets::load_from_json(VisualGraph &vg, const std::string &filepath, std::string &error_msg) {
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        error_msg = "Could not open file for reading: " + filepath;
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        error_msg = "Empty file: " + filepath;
        return false;
    }

    VisualGraph new_vg;
    std::size_t nodes_pos = content.find("\"nodes\"");
    std::size_t edges_pos = content.find("\"edges\"");

    if (nodes_pos == std::string::npos) {
        error_msg = "Missing 'nodes' array in JSON";
        return false;
    }

    // Simple robust tokenizer for JSON nodes
    std::size_t cur = nodes_pos;
    while (cur < edges_pos && cur < content.size()) {
        std::size_t x_pos = content.find("\"x\":", cur);
        if (x_pos == std::string::npos || (edges_pos != std::string::npos && x_pos > edges_pos)) break;
        std::size_t y_pos = content.find("\"y\":", x_pos);
        if (y_pos == std::string::npos || (edges_pos != std::string::npos && y_pos > edges_pos)) break;

        double x = std::stod(content.substr(x_pos + 4));
        double y = std::stod(content.substr(y_pos + 4));
        new_vg.add_node(static_cast<float>(x), static_cast<float>(y));
        cur = y_pos + 4;
    }

    // Tokenizer for JSON edges
    if (edges_pos != std::string::npos) {
        cur = edges_pos;
        while (cur < content.size()) {
            std::size_t from_pos = content.find("\"from\":", cur);
            if (from_pos == std::string::npos) break;
            std::size_t to_pos = content.find("\"to\":", from_pos);
            if (to_pos == std::string::npos) break;
            std::size_t weight_pos = content.find("\"weight\":", to_pos);
            if (weight_pos == std::string::npos) break;

            std::size_t u = std::stoul(content.substr(from_pos + 7));
            std::size_t v = std::stoul(content.substr(to_pos + 5));
            double w = std::stod(content.substr(weight_pos + 9));

            new_vg.add_edge(u, v, w);
            cur = weight_pos + 9;
        }
    }

    vg = std::move(new_vg);
    return true;
}

} // namespace dijkstra::gui
