/**
 * @file canvas.cpp
 * @brief Implementation of VisualGraph and Canvas.
 */

#include "canvas.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>

namespace dijkstra::gui {

namespace {

constexpr ImU32 col32(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255) noexcept {
    return (a << 24) | (b << 16) | (g << 8) | r;
}

float point_distance_sq(const ImVec2 &p1, const ImVec2 &p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return dx * dx + dy * dy;
}

float distance_to_segment(const ImVec2 &p, const ImVec2 &a, const ImVec2 &b) {
    float l2 = point_distance_sq(a, b);
    if (l2 == 0.0f) {
        return std::sqrt(point_distance_sq(p, a));
    }
    float t = ((p.x - a.x) * (b.x - a.x) + (p.y - a.y) * (b.y - a.y)) / l2;
    t = std::clamp(t, 0.0f, 1.0f);
    ImVec2 projection(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y));
    return std::sqrt(point_distance_sq(p, projection));
}

std::string format_float(double val) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << val;
    std::string s = oss.str();
    if (s.size() >= 2 && s.substr(s.size() - 2) == ".0") {
        s.erase(s.size() - 2);
    }
    return s;
}

} // namespace

VisualGraph::VisualGraph(std::size_t node_count)
    : graph_(node_count, false), positions_(node_count) {
    if (node_count > 0) {
        const float radius = 180.0f;
        const float center_x = 400.0f;
        const float center_y = 300.0f;
        for (std::size_t i = 0; i < node_count; ++i) {
            double angle = 2.0 * 3.14159265358979323846 * static_cast<double>(i) / static_cast<double>(node_count);
            positions_[i] = Point{
                center_x + radius * std::cos(angle),
                center_y + radius * std::sin(angle)
            };
        }
    }
}

NodeId VisualGraph::add_node(float x, float y) {
    NodeId new_id = graph_.node_count();
    graph_.resize(new_id + 1);
    positions_.push_back(Point{static_cast<double>(x), static_cast<double>(y)});
    return new_id;
}

void VisualGraph::remove_node(NodeId id) {
    if (id >= graph_.node_count()) {
        return;
    }
    std::size_t n = graph_.node_count();
    Graph new_g(n - 1, graph_.is_directed());
    std::vector<Point> new_pos;
    new_pos.reserve(n - 1);

    auto map_id = [id](NodeId old_id) -> NodeId {
        return old_id < id ? old_id : old_id - 1;
    };

    for (NodeId u = 0; u < n; ++u) {
        if (u == id) continue;
        new_pos.push_back(positions_[u]);
        for (const auto &edge : graph_.neighbors(u)) {
            if (edge.to != id && edge.from <= edge.to) {
                new_g.add_edge(map_id(edge.from), map_id(edge.to), edge.weight);
            }
        }
    }

    graph_ = std::move(new_g);
    positions_ = std::move(new_pos);
}

void VisualGraph::add_edge(NodeId u, NodeId v, Weight weight) {
    if (u < graph_.node_count() && v < graph_.node_count()) {
        graph_.add_edge(u, v, weight);
    }
}

void VisualGraph::remove_edge(NodeId u, NodeId v) {
    if (u < graph_.node_count() && v < graph_.node_count()) {
        graph_.add_edge(u, v, 0.0);
    }
}

void VisualGraph::set_node_position(NodeId id, float x, float y) {
    if (id < positions_.size()) {
        positions_[id] = Point{static_cast<double>(x), static_cast<double>(y)};
    }
}

void VisualGraph::clear() {
    graph_.clear();
    positions_.clear();
}

void VisualGraph::load_from_graph(const Graph &g, const std::vector<Point> &pos) {
    graph_ = g;
    std::size_t n = g.node_count();
    if (pos.size() == n) {
        positions_ = pos;
    } else {
        positions_.resize(n);
        const float radius = 180.0f;
        const float center_x = 400.0f;
        const float center_y = 300.0f;
        for (std::size_t i = 0; i < n; ++i) {
            double angle = 2.0 * 3.14159265358979323846 * static_cast<double>(i) / static_cast<double>(n);
            positions_[i] = Point{
                center_x + radius * std::cos(angle),
                center_y + radius * std::sin(angle)
            };
        }
    }
}

Canvas::Canvas() : pan_(0.0f, 0.0f), zoom_(1.0f) {}

ImVec2 Canvas::world_to_screen(const ImVec2 &world_pos, const ImVec2 &origin) const {
    return ImVec2(origin.x + pan_.x + world_pos.x * zoom_, origin.y + pan_.y + world_pos.y * zoom_);
}

ImVec2 Canvas::screen_to_world(const ImVec2 &screen_pos, const ImVec2 &origin) const {
    return ImVec2((screen_pos.x - origin.x - pan_.x) / zoom_, (screen_pos.y - origin.y - pan_.y) / zoom_);
}

std::optional<NodeId> Canvas::hit_test_node(
    const VisualGraph &vg,
    const ImVec2 &world_pos,
    float radius
) const {
    const auto &positions = vg.positions();
    float r2 = radius * radius;
    for (std::size_t i = 0; i < positions.size(); ++i) {
        ImVec2 node_pos(static_cast<float>(positions[i].x), static_cast<float>(positions[i].y));
        if (point_distance_sq(world_pos, node_pos) <= r2) {
            return i;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<NodeId, NodeId>> Canvas::hit_test_edge(
    const VisualGraph &vg,
    const ImVec2 &world_pos,
    float threshold
) const {
    const auto &positions = vg.positions();
    const auto edges = vg.graph().all_edges();

    std::optional<std::pair<NodeId, NodeId>> closest_edge = std::nullopt;
    float min_dist = threshold;

    for (const auto &edge : edges) {
        if (edge.from >= positions.size() || edge.to >= positions.size()) {
            continue;
        }
        ImVec2 a(static_cast<float>(positions[edge.from].x), static_cast<float>(positions[edge.from].y));
        ImVec2 b(static_cast<float>(positions[edge.to].x), static_cast<float>(positions[edge.to].y));

        float d = distance_to_segment(world_pos, a, b);
        if (d < min_dist) {
            min_dist = d;
            closest_edge = std::make_pair(edge.from, edge.to);
        }
    }

    return closest_edge;
}

void Canvas::reset_view() {
    pan_ = ImVec2(0.0f, 0.0f);
    zoom_ = 1.0f;
}

void Canvas::center_on_graph(const VisualGraph &vg, const ImVec2 &canvas_size) {
    if (vg.node_count() == 0) {
        reset_view();
        return;
    }
    float min_x = 1e9f, min_y = 1e9f, max_x = -1e9f, max_y = -1e9f;
    for (const auto &p : vg.positions()) {
        min_x = std::min(min_x, static_cast<float>(p.x));
        min_y = std::min(min_y, static_cast<float>(p.y));
        max_x = std::max(max_x, static_cast<float>(p.x));
        max_y = std::max(max_y, static_cast<float>(p.y));
    }
    float center_x = (min_x + max_x) * 0.5f;
    float center_y = (min_y + max_y) * 0.5f;

    zoom_ = 1.0f;
    pan_ = ImVec2(canvas_size.x * 0.5f - center_x * zoom_, canvas_size.y * 0.5f - center_y * zoom_);
}

void Canvas::render(
    VisualGraph &vg,
    const ImVec2 &canvas_pos,
    const ImVec2 &canvas_size,
    const std::optional<DijkstraStep> &current_step,
    const std::optional<DijkstraResult> &instant_result
) {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImGuiIO &io = ImGui::GetIO();

    ImVec2 canvas_p0 = canvas_pos;
    ImVec2 canvas_p1 = ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y);

    draw_list->AddRectFilled(canvas_p0, canvas_p1, col32(24, 26, 32, 255));
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    // 1. Draw Grid
    const float grid_step = 64.0f * zoom_;
    float grid_offset_x = std::fmod(pan_.x, grid_step);
    float grid_offset_y = std::fmod(pan_.y, grid_step);
    for (float x = grid_offset_x; x < canvas_size.x; x += grid_step) {
        draw_list->AddLine(
            ImVec2(canvas_p0.x + x, canvas_p0.y),
            ImVec2(canvas_p0.x + x, canvas_p1.y),
            col32(40, 44, 52, 200)
        );
    }
    for (float y = grid_offset_y; y < canvas_size.y; y += grid_step) {
        draw_list->AddLine(
            ImVec2(canvas_p0.x, canvas_p0.y + y),
            ImVec2(canvas_p1.x, canvas_p0.y + y),
            col32(40, 44, 52, 200)
        );
    }

    // Handle mouse interactions inside canvas
    bool is_canvas_hovered = ImGui::IsMouseHoveringRect(canvas_p0, canvas_p1);
    ImVec2 mouse_screen = io.MousePos;
    ImVec2 mouse_world = screen_to_world(mouse_screen, canvas_p0);

    hovered_node_ = std::nullopt;
    hovered_edge_ = std::nullopt;

    if (is_canvas_hovered) {
        hovered_node_ = hit_test_node(vg, mouse_world, 24.0f);
        if (!hovered_node_.has_value()) {
            hovered_edge_ = hit_test_edge(vg, mouse_world, 10.0f);
        }

        // Pan with middle or right drag (when not interacting with node)
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, 2.0f) ||
            ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 2.0f)) {
            pan_.x += io.MouseDelta.x;
            pan_.y += io.MouseDelta.y;
        }

        // Zoom with wheel
        if (io.MouseWheel != 0.0f) {
            float zoom_delta = io.MouseWheel * 0.1f;
            float new_zoom = std::clamp(zoom_ + zoom_delta, 0.2f, 3.0f);
            ImVec2 mouse_before_zoom = screen_to_world(mouse_screen, canvas_p0);
            zoom_ = new_zoom;
            ImVec2 mouse_after_zoom = screen_to_world(mouse_screen, canvas_p0);
            pan_.x += (mouse_after_zoom.x - mouse_before_zoom.x) * zoom_;
            pan_.y += (mouse_after_zoom.y - mouse_before_zoom.y) * zoom_;
        }

        // Double click on empty canvas to create node
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !hovered_node_.has_value()) {
            vg.add_node(mouse_world.x, mouse_world.y);
            is_modified_ = true;
        }

        // Shift + Click or Drag to connect nodes
        if (io.KeyShift) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hovered_node_.has_value()) {
                connecting_source_ = hovered_node_;
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && connecting_source_.has_value()) {
                if (hovered_node_.has_value() && *hovered_node_ != *connecting_source_) {
                    NodeId u = *connecting_source_;
                    NodeId v = *hovered_node_;
                    float d = std::sqrt(point_distance_sq(
                        ImVec2(static_cast<float>(vg.positions()[u].x), static_cast<float>(vg.positions()[u].y)),
                        ImVec2(static_cast<float>(vg.positions()[v].x), static_cast<float>(vg.positions()[v].y))
                    ));
                    // Round to 1 decimal
                    Weight w = std::round(static_cast<double>(d) * 0.1) * 0.1;
                    if (w < 1.0) w = 1.0;
                    vg.add_edge(u, v, w);
                    is_modified_ = true;
                }
                connecting_source_ = std::nullopt;
            }
        } else {
            connecting_source_ = std::nullopt;

            // Dragging node
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hovered_node_.has_value()) {
                dragging_node_ = hovered_node_;
            }
            if (dragging_node_.has_value()) {
                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 1.0f)) {
                    vg.set_node_position(*dragging_node_, mouse_world.x, mouse_world.y);
                    is_modified_ = true;
                }
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    // Clicked without dragging: select Start / Target
                    if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left, 2.0f)) {
                        NodeId clicked = *dragging_node_;
                        if (!selected_start_.has_value()) {
                            selected_start_ = clicked;
                        } else if (selected_start_ == clicked) {
                            selected_start_ = std::nullopt;
                            selected_target_ = std::nullopt;
                        } else if (!selected_target_.has_value()) {
                            selected_target_ = clicked;
                        } else if (selected_target_ == clicked) {
                            selected_target_ = std::nullopt;
                        } else {
                            selected_start_ = clicked;
                            selected_target_ = std::nullopt;
                        }
                        is_modified_ = true;
                    }
                    dragging_node_ = std::nullopt;
                }
            }
        }

        // Open context menus on right click
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsMouseDragging(ImGuiMouseButton_Right, 2.0f)) {
            if (hovered_node_.has_value()) {
                popup_node_ = hovered_node_;
                ImGui::OpenPopup("NodeContextMenu");
            } else if (hovered_edge_.has_value()) {
                popup_edge_ = hovered_edge_;
                edit_weight_buffer_ = static_cast<float>(vg.graph().get_weight(popup_edge_->first, popup_edge_->second));
                ImGui::OpenPopup("EdgeContextMenu");
            }
        }
    }

    // Context Menus
    if (ImGui::BeginPopup("NodeContextMenu")) {
        if (popup_node_.has_value()) {
            ImGui::Text("Vertex #%zu", *popup_node_);
            ImGui::Separator();
            if (ImGui::MenuItem("Set as Start (Source)")) {
                selected_start_ = popup_node_;
                is_modified_ = true;
            }
            if (ImGui::MenuItem("Set as Target (Destination)")) {
                selected_target_ = popup_node_;
                is_modified_ = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete Vertex")) {
                vg.remove_node(*popup_node_);
                if (selected_start_ == popup_node_) selected_start_ = std::nullopt;
                if (selected_target_ == popup_node_) selected_target_ = std::nullopt;
                popup_node_ = std::nullopt;
                is_modified_ = true;
            }
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("EdgeContextMenu")) {
        if (popup_edge_.has_value()) {
            ImGui::Text("Edge (%zu -- %zu)", popup_edge_->first, popup_edge_->second);
            ImGui::Separator();
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::InputFloat("Weight", &edit_weight_buffer_, 1.0f, 5.0f, "%.1f")) {
                if (edit_weight_buffer_ > 0.0f) {
                    vg.add_edge(popup_edge_->first, popup_edge_->second, edit_weight_buffer_);
                    is_modified_ = true;
                }
            }
            if (ImGui::MenuItem("Delete Edge")) {
                vg.remove_edge(popup_edge_->first, popup_edge_->second);
                popup_edge_ = std::nullopt;
                is_modified_ = true;
            }
        }
        ImGui::EndPopup();
    }

    // Collect active shortest path edges for highlighting
    std::vector<std::pair<NodeId, NodeId>> path_edges;
    if (current_step.has_value() && !current_step->active_path.empty()) {
        const auto &p = current_step->active_path;
        for (std::size_t i = 0; i + 1 < p.size(); ++i) {
            path_edges.emplace_back(p[i], p[i + 1]);
        }
    } else if (instant_result.has_value() && selected_target_.has_value()) {
        auto p = instant_result->path_to(*selected_target_);
        if (p) {
            for (std::size_t i = 0; i + 1 < p->size(); ++i) {
                path_edges.emplace_back((*p)[i], (*p)[i + 1]);
            }
        }
    }

    auto is_path_edge = [&](NodeId u, NodeId v) -> bool {
        for (const auto &pe : path_edges) {
            if ((pe.first == u && pe.second == v) || (pe.first == v && pe.second == u)) {
                return true;
            }
        }
        return false;
    };

    // 2. Render Edges
    const auto &positions = vg.positions();
    const auto edges = vg.graph().all_edges();

    for (const auto &edge : edges) {
        if (edge.from >= positions.size() || edge.to >= positions.size()) continue;

        ImVec2 p1 = world_to_screen(ImVec2(static_cast<float>(positions[edge.from].x), static_cast<float>(positions[edge.from].y)), canvas_p0);
        ImVec2 p2 = world_to_screen(ImVec2(static_cast<float>(positions[edge.to].x), static_cast<float>(positions[edge.to].y)), canvas_p0);

        bool in_path = is_path_edge(edge.from, edge.to);
        bool is_active_step_edge = current_step.has_value() &&
                                  ((current_step->current_node == edge.from && current_step->neighbor_node == edge.to) ||
                                   (current_step->current_node == edge.to && current_step->neighbor_node == edge.from));

        ImU32 edge_color = col32(100, 110, 130, 200);
        float thickness = 2.0f * zoom_;

        if (in_path) {
            edge_color = col32(245, 185, 60, 255); // Golden path
            thickness = 4.5f * zoom_;
        } else if (is_active_step_edge) {
            edge_color = col32(255, 120, 40, 255); // Active step relaxation
            thickness = 3.5f * zoom_;
        } else if (hovered_edge_.has_value() &&
                   ((hovered_edge_->first == edge.from && hovered_edge_->second == edge.to) ||
                    (hovered_edge_->first == edge.to && hovered_edge_->second == edge.from))) {
            edge_color = col32(180, 195, 220, 255);
            thickness = 3.0f * zoom_;
        }

        draw_list->AddLine(p1, p2, edge_color, thickness);

        // Edge Weight Badge
        ImVec2 mid((p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f);
        std::string weight_str = format_float(edge.weight);
        ImVec2 text_size = ImGui::CalcTextSize(weight_str.c_str());
        ImVec2 badge_pad(5.0f, 2.0f);
        ImVec2 b_min(mid.x - text_size.x * 0.5f - badge_pad.x, mid.y - text_size.y * 0.5f - badge_pad.y);
        ImVec2 b_max(mid.x + text_size.x * 0.5f + badge_pad.x, mid.y + text_size.y * 0.5f + badge_pad.y);

        draw_list->AddRectFilled(b_min, b_max, col32(30, 34, 42, 230), 4.0f);
        draw_list->AddRect(b_min, b_max, in_path ? col32(245, 185, 60, 255) : col32(70, 78, 92, 255), 4.0f);
        draw_list->AddText(ImVec2(mid.x - text_size.x * 0.5f, mid.y - text_size.y * 0.5f), col32(220, 225, 235, 255), weight_str.c_str());
    }

    // Connecting line preview
    if (connecting_source_.has_value() && *connecting_source_ < positions.size()) {
        ImVec2 p_src = world_to_screen(ImVec2(static_cast<float>(positions[*connecting_source_].x), static_cast<float>(positions[*connecting_source_].y)), canvas_p0);
        draw_list->AddLine(p_src, mouse_screen, col32(75, 175, 255, 200), 2.0f * zoom_);
    }

    // 3. Render Nodes
    const float base_radius = 20.0f * zoom_;
    for (std::size_t i = 0; i < positions.size(); ++i) {
        ImVec2 p = world_to_screen(ImVec2(static_cast<float>(positions[i].x), static_cast<float>(positions[i].y)), canvas_p0);

        // Determine node color state
        ImU32 fill_color = col32(45, 52, 68, 255);
        ImU32 border_color = col32(110, 125, 150, 255);
        float border_width = 2.0f * zoom_;

        if (current_step.has_value() && i < current_step->node_states.size()) {
            switch (current_step->node_states[i]) {
                case NodeVisualState::IN_PATH:
                    fill_color = col32(230, 165, 30, 255); // Golden in-path
                    border_color = col32(255, 220, 120, 255);
                    border_width = 3.5f * zoom_;
                    break;
                case NodeVisualState::SETTLED:
                    fill_color = col32(40, 110, 210, 255); // Sky blue settled
                    border_color = col32(90, 160, 255, 255);
                    break;
                case NodeVisualState::OPEN:
                    fill_color = col32(235, 120, 30, 255); // Orange in priority queue
                    border_color = col32(255, 180, 80, 255);
                    break;
                case NodeVisualState::UNVISITED:
                    fill_color = col32(45, 52, 68, 255);
                    border_color = col32(100, 115, 140, 255);
                    break;
            }
        } else if (instant_result.has_value()) {
            if (instant_result->has_path_to(i)) {
                fill_color = col32(40, 110, 210, 255);
                border_color = col32(90, 160, 255, 255);
            }
        }

        // Start & Target overrides
        if (selected_start_.has_value() && *selected_start_ == i) {
            fill_color = col32(35, 165, 90, 255); // Emerald green for start
            border_color = col32(95, 235, 150, 255);
            border_width = 3.5f * zoom_;
        } else if (selected_target_.has_value() && *selected_target_ == i) {
            fill_color = col32(215, 50, 60, 255); // Crimson red for target
            border_color = col32(255, 120, 130, 255);
            border_width = 3.5f * zoom_;
        } else if (hovered_node_.has_value() && *hovered_node_ == i) {
            border_color = col32(255, 255, 255, 255);
        }

        // Draw node circle
        draw_list->AddCircleFilled(p, base_radius, fill_color, 32);
        draw_list->AddCircle(p, base_radius, border_color, 32, border_width);

        // Node ID Text
        std::string id_str = std::to_string(i);
        ImVec2 text_size = ImGui::CalcTextSize(id_str.c_str());
        draw_list->AddText(
            ImVec2(p.x - text_size.x * 0.5f, p.y - text_size.y * 0.5f),
            col32(255, 255, 255, 255),
            id_str.c_str()
        );

        // Optional Distance Badge above node
        if (current_step.has_value() && i < current_step->distances.size()) {
            Weight d = current_step->distances[i];
            if (d < kInfinity) {
                std::string dist_str = "[" + format_float(d) + "]";
                ImVec2 d_size = ImGui::CalcTextSize(dist_str.c_str());
                draw_list->AddText(
                    ImVec2(p.x - d_size.x * 0.5f, p.y - base_radius - d_size.y - 2.0f),
                    col32(200, 220, 255, 240),
                    dist_str.c_str()
                );
            }
        } else if (instant_result.has_value() && instant_result->has_path_to(i)) {
            Weight d = *instant_result->distance_to(i);
            std::string dist_str = "[" + format_float(d) + "]";
            ImVec2 d_size = ImGui::CalcTextSize(dist_str.c_str());
            draw_list->AddText(
                ImVec2(p.x - d_size.x * 0.5f, p.y - base_radius - d_size.y - 2.0f),
                col32(200, 220, 255, 240),
                dist_str.c_str()
            );
        }
    }

    draw_list->PopClipRect();
}

} // namespace dijkstra::gui
