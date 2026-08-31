/**
 * @file canvas.h
 * @brief Interactive 2D Graph Canvas and visual editor with Dear ImGui.
 */

#pragma once

#include "dijkstra/dijkstra.h"
#include "dijkstra/graph.h"
#include "dijkstra/point.h"
#include "dijkstra/types.h"
#include "step_tracer.h"

#include <imgui.h>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace dijkstra::gui {

/**
 * @brief Represents a graph with explicit 2D spatial positions for visualization and editing.
 */
class VisualGraph {
  public:
    explicit VisualGraph(std::size_t node_count = 0);

    [[nodiscard]] const Graph &graph() const noexcept {
        return graph_;
    }

    [[nodiscard]] Graph &graph() noexcept {
        return graph_;
    }

    [[nodiscard]] const std::vector<Point> &positions() const noexcept {
        return positions_;
    }

    [[nodiscard]] std::size_t node_count() const noexcept {
        return graph_.node_count();
    }

    NodeId add_node(float x, float y);
    void remove_node(NodeId id);
    void add_edge(NodeId u, NodeId v, Weight weight);
    void remove_edge(NodeId u, NodeId v);
    void set_node_position(NodeId id, float x, float y);
    void clear();

    void load_from_graph(const Graph &g, const std::vector<Point> &pos = {});

  private:
    Graph graph_;
    std::vector<Point> positions_;
};

/**
 * @brief Interactive 2D Canvas supporting mouse interactions, Pan/Zoom, and ImDrawList rendering.
 */
class Canvas {
  public:
    Canvas();

    // Coordinate conversions
    [[nodiscard]] ImVec2 world_to_screen(const ImVec2 &world_pos, const ImVec2 &origin) const;
    [[nodiscard]] ImVec2 screen_to_world(const ImVec2 &screen_pos, const ImVec2 &origin) const;

    // Hit-testing queries
    [[nodiscard]] std::optional<NodeId> hit_test_node(
        const VisualGraph &vg,
        const ImVec2 &world_pos,
        float radius = 24.0f
    ) const;

    [[nodiscard]] std::optional<std::pair<NodeId, NodeId>> hit_test_edge(
        const VisualGraph &vg,
        const ImVec2 &world_pos,
        float threshold = 10.0f
    ) const;

    // Rendering and interaction dispatch
    void render(
        VisualGraph &vg,
        const ImVec2 &canvas_pos,
        const ImVec2 &canvas_size,
        const std::optional<DijkstraStep> &current_step = std::nullopt,
        const std::optional<DijkstraResult> &instant_result = std::nullopt
    );

    // Selection accessors
    [[nodiscard]] std::optional<NodeId> selected_start() const noexcept {
        return selected_start_;
    }
    [[nodiscard]] std::optional<NodeId> selected_target() const noexcept {
        return selected_target_;
    }
    void set_selected_start(std::optional<NodeId> id) noexcept {
        selected_start_ = id;
        is_modified_ = true;
    }
    void set_selected_target(std::optional<NodeId> id) noexcept {
        selected_target_ = id;
        is_modified_ = true;
    }
    void clear_selection() noexcept {
        selected_start_ = std::nullopt;
        selected_target_ = std::nullopt;
        is_modified_ = true;
    }

    [[nodiscard]] bool consume_modified() noexcept {
        bool m = is_modified_;
        is_modified_ = false;
        return m;
    }
    void mark_modified() noexcept {
        is_modified_ = true;
    }

    // Viewport reset
    void reset_view();
    void center_on_graph(const VisualGraph &vg, const ImVec2 &canvas_size);

    [[nodiscard]] ImVec2 pan() const noexcept {
        return pan_;
    }
    [[nodiscard]] float zoom() const noexcept {
        return zoom_;
    }

  private:
    ImVec2 pan_{0.0f, 0.0f};
    float zoom_{1.0f};

    std::optional<NodeId> hovered_node_{std::nullopt};
    std::optional<std::pair<NodeId, NodeId>> hovered_edge_{std::nullopt};
    std::optional<NodeId> dragging_node_{std::nullopt};
    std::optional<NodeId> connecting_source_{std::nullopt};

    std::optional<NodeId> selected_start_{std::nullopt};
    std::optional<NodeId> selected_target_{std::nullopt};

    // Popup context menu states
    std::optional<NodeId> popup_node_{std::nullopt};
    std::optional<std::pair<NodeId, NodeId>> popup_edge_{std::nullopt};
    float edit_weight_buffer_{1.0f};

    bool is_modified_{true};
};

} // namespace dijkstra::gui
