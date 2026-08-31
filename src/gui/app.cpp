/**
 * @file app.cpp
 * @brief Implementation of GUI Application and HUD controllers.
 */

#include "app.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace dijkstra::gui {

namespace {

std::string format_time_us(double us) {
    std::ostringstream oss;
    if (us < 1000.0) {
        oss << std::fixed << std::setprecision(1) << us << " us";
    } else {
        oss << std::fixed << std::setprecision(3) << (us / 1000.0) << " ms";
    }
    return oss.str();
}

std::string format_dist(double d) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << d;
    return oss.str();
}

} // namespace

App::App() {
    load_preset("wikipedia");
    canvas_.set_selected_start(0);
    canvas_.set_selected_target(4);
    solve_instant();
}

const std::optional<DijkstraStep> &App::current_step() const {
    if (mode_ == AppMode::ANIMATED &&
        current_step_idx_ >= 0 &&
        current_step_idx_ < static_cast<int>(animation_steps_.size())) {
        current_step_cached_ = animation_steps_[static_cast<std::size_t>(current_step_idx_)];
    } else {
        current_step_cached_ = std::nullopt;
    }
    return current_step_cached_;
}

void App::update(float dt) {
    if (mode_ == AppMode::ANIMATED && is_playing_) {
        step_timer_ += dt;
        if (step_timer_ >= step_interval_) {
            step_timer_ = 0.0f;
            if (current_step_idx_ + 1 < static_cast<int>(animation_steps_.size())) {
                step_forward();
            } else {
                is_playing_ = false;
            }
        }
    }
}

void App::solve_instant() {
    auto start = canvas_.selected_start();
    if (!start.has_value() || *start >= vg_.node_count()) {
        instant_result_ = std::nullopt;
        hud_stats_ = HudStatistics{
            .elapsed_us = 0.0,
            .total_nodes = vg_.node_count(),
            .total_edges = vg_.graph().edge_count(),
            .settled_nodes = 0,
            .relaxation_count = 0,
            .total_distance = std::nullopt,
            .path_nodes = {}
        };
        return;
    }

    auto target = canvas_.selected_target();

    const auto t0 = std::chrono::high_resolution_clock::now();

    DijkstraResult res = target.has_value()
        ? shortest_path(vg_.graph(), *start, *target)
        : shortest_paths(vg_.graph(), *start);

    const auto t1 = std::chrono::high_resolution_clock::now();
    const double elapsed_us = std::chrono::duration<double, std::micro>(t1 - t0).count();

    std::optional<Weight> dist = std::nullopt;
    std::vector<NodeId> path;

    if (target.has_value() && res.has_path_to(*target)) {
        dist = res.distance_to(*target);
        path = *res.path_to(*target);
    }

    hud_stats_ = HudStatistics{
        .elapsed_us = elapsed_us,
        .total_nodes = vg_.node_count(),
        .total_edges = vg_.graph().edge_count(),
        .settled_nodes = res.visited_count(),
        .relaxation_count = 0,
        .total_distance = dist,
        .path_nodes = path
    };

    instant_result_ = std::move(res);
}

void App::start_animation() {
    auto start = canvas_.selected_start();
    if (!start.has_value() || *start >= vg_.node_count()) {
        status_message_ = "Select a start vertex to begin animation.";
        return;
    }

    animation_steps_ = StepTracer::trace(vg_.graph(), *start, canvas_.selected_target());
    if (animation_steps_.empty()) {
        status_message_ = "Could not trace steps for current graph.";
        return;
    }

    current_step_idx_ = 0;
    is_playing_ = true;
    step_timer_ = 0.0f;
    mode_ = AppMode::ANIMATED;
}

void App::play() noexcept {
    if (animation_steps_.empty()) {
        start_animation();
    } else {
        is_playing_ = true;
    }
}

void App::pause() noexcept {
    is_playing_ = false;
}

void App::toggle_play() noexcept {
    if (is_playing_) {
        pause();
    } else {
        play();
    }
}

void App::step_forward() {
    if (current_step_idx_ + 1 < static_cast<int>(animation_steps_.size())) {
        ++current_step_idx_;
    }
}

void App::step_backward() {
    if (current_step_idx_ > 0) {
        --current_step_idx_;
    }
}

void App::reset_playback() {
    is_playing_ = false;
    current_step_idx_ = 0;
    step_timer_ = 0.0f;
}

void App::set_step_index(int idx) {
    if (idx >= 0 && idx < static_cast<int>(animation_steps_.size())) {
        current_step_idx_ = idx;
    }
}

void App::load_preset(const std::string &name) {
    if (name == "wikipedia") {
        vg_ = Presets::wikipedia_sample();
    } else if (name == "ring") {
        vg_ = Presets::ring_graph(8, 180.0f);
    } else if (name == "star") {
        vg_ = Presets::star_graph(7, 180.0f);
    } else if (name == "grid") {
        vg_ = Presets::grid_maze(4, 5, 85.0f);
    } else if (name == "random") {
        vg_ = Presets::random_geometric(10, 3, 500.0f, 350.0f);
    }

    canvas_.reset_view();
    if (mode_ == AppMode::INSTANT) {
        solve_instant();
    } else {
        start_animation();
    }
}

void App::clear_graph() {
    vg_.clear();
    canvas_.clear_selection();
    instant_result_ = std::nullopt;
    animation_steps_.clear();
    current_step_idx_ = -1;
    hud_stats_ = HudStatistics{};
}

void App::simulate_double_click(float world_x, float world_y) {
    vg_.add_node(world_x, world_y);
}

void App::simulate_connect_nodes(NodeId u, NodeId v, Weight w) {
    vg_.add_edge(u, v, w);
}

void App::simulate_set_selection(std::optional<NodeId> start, std::optional<NodeId> target) {
    canvas_.set_selected_start(start);
    canvas_.set_selected_target(target);
    solve_instant();
}

void App::render_ui() {
    // 1. Fullscreen Main Viewport Docking Space
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                   ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                                   ImGuiWindowFlags_NoNavFocus |
                                   ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("MainWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    render_top_toolbar();

    // Render Canvas in remaining space
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
    if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;

    // Solve only when graph or selection changed
    if (mode_ == AppMode::INSTANT && (canvas_.consume_modified() || is_dirty_)) {
        solve_instant();
        is_dirty_ = false;
    }

    canvas_.render(
        vg_,
        canvas_pos,
        canvas_size,
        current_step(),
        instant_result_
    );

    ImGui::End();

    // 2. Render Overlays
    render_hud_overlay();
    if (mode_ == AppMode::ANIMATED) {
        render_playback_bar();
    }

    if (show_help_modal_) {
        render_help_modal();
    }
    if (show_random_modal_) {
        render_random_modal();
    }
}

void App::render_top_toolbar() {
    if (ImGui::BeginMenuBar()) {
        // Algorithm Mode
        if (ImGui::RadioButton("Instant (Fast)", mode_ == AppMode::INSTANT)) {
            mode_ = AppMode::INSTANT;
            solve_instant();
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Step Animation", mode_ == AppMode::ANIMATED)) {
            mode_ = AppMode::ANIMATED;
            start_animation();
        }

        ImGui::Separator();

        // Presets Menu
        if (ImGui::BeginMenu("Presets")) {
            if (ImGui::MenuItem("Wikipedia (6 Nodes)")) load_preset("wikipedia");
            if (ImGui::MenuItem("Ring Topology")) load_preset("ring");
            if (ImGui::MenuItem("Star Topology")) load_preset("star");
            if (ImGui::MenuItem("Grid Maze")) load_preset("grid");
            if (ImGui::MenuItem("Random Geometric...")) show_random_modal_ = true;
            ImGui::EndMenu();
        }

        // File Menu
        if (ImGui::BeginMenu("Map")) {
            if (ImGui::MenuItem("Save to JSON...")) {
                std::string err;
                if (!Presets::save_to_json(vg_, file_path_buffer_, err)) {
                    status_message_ = "Error saving: " + err;
                } else {
                    status_message_ = "Map successfully saved to " + std::string(file_path_buffer_);
                }
            }
            if (ImGui::MenuItem("Load from JSON...")) {
                std::string err;
                if (!Presets::load_from_json(vg_, file_path_buffer_, err)) {
                    status_message_ = "Error loading: " + err;
                } else {
                    status_message_ = "Map loaded from " + std::string(file_path_buffer_);
                    if (mode_ == AppMode::INSTANT) solve_instant();
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear Canvas")) clear_graph();
            ImGui::EndMenu();
        }

        // Quick Actions
        if (ImGui::Button("Random Graph")) {
            show_random_modal_ = true;
        }

        if (ImGui::Button("Center View")) {
            canvas_.center_on_graph(vg_, ImGui::GetIO().DisplaySize);
        }

        // Help Button at right
        float help_btn_width = 32.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - help_btn_width - 15.0f);
        if (ImGui::Button("(i)")) {
            show_help_modal_ = true;
        }

        ImGui::EndMenuBar();
    }
}

void App::render_playback_bar() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    const float bar_width = 720.0f;
    const float bar_height = 95.0f;
    ImVec2 pos(
        viewport->WorkPos.x + (viewport->WorkSize.x - bar_width) * 0.5f,
        viewport->WorkPos.y + viewport->WorkSize.y - bar_height - 20.0f
    );

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(bar_width, bar_height), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.12f, 0.14f, 0.18f, 0.92f));

    if (ImGui::Begin("PlaybackBar", nullptr, flags)) {
        // Controls Row
        if (ImGui::Button("Reset")) reset_playback();
        ImGui::SameLine();
        if (ImGui::Button("Prev")) step_backward();
        ImGui::SameLine();
        if (ImGui::Button(is_playing_ ? "Pause" : "Play")) toggle_play();
        ImGui::SameLine();
        if (ImGui::Button("Next")) step_forward();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(140.0f);
        ImGui::SliderFloat("Speed", &step_interval_, 0.05f, 1.0f, "%.2fs / step");

        // Step Scrubber
        int total = static_cast<int>(animation_steps_.size());
        int current = current_step_idx_ + 1;
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        if (ImGui::SliderInt("Step", &current, 1, std::max(1, total), "%d / %d")) {
            current_step_idx_ = current - 1;
        }

        // Action Description
        if (current_step_idx_ >= 0 && current_step_idx_ < total) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.95f, 0.75f, 0.25f, 1.0f), "%s", animation_steps_[static_cast<std::size_t>(current_step_idx_)].description.c_str());
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void App::render_hud_overlay() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 pos(viewport->WorkPos.x + 15.0f, viewport->WorkPos.y + 40.0f);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(280.0f, 0.0f), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.12f, 0.15f, 0.88f));

    if (ImGui::Begin("HudStats", nullptr, flags)) {
        ImGui::TextColored(ImVec4(0.35f, 0.75f, 1.0f, 1.0f), "Execution Metrics");
        ImGui::Separator();

        ImGui::Text("Vertices / Edges: %zu / %zu", hud_stats_.total_nodes, hud_stats_.total_edges);
        ImGui::Text("Computation Time: %s", format_time_us(hud_stats_.elapsed_us).c_str());
        ImGui::Text("Settled Vertices: %zu / %zu", hud_stats_.settled_nodes, hud_stats_.total_nodes);

        if (canvas_.selected_start().has_value()) {
            ImGui::Text("Start Vertex: %zu", *canvas_.selected_start());
        } else {
            ImGui::TextDisabled("Start Vertex: [None]");
        }

        if (canvas_.selected_target().has_value()) {
            ImGui::Text("Target Vertex: %zu", *canvas_.selected_target());
        } else {
            ImGui::TextDisabled("Target Vertex: [All reachable]");
        }

        ImGui::Separator();

        if (hud_stats_.total_distance.has_value()) {
            ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.2f, 1.0f), "Shortest Path: %s", format_dist(*hud_stats_.total_distance).c_str());
            if (!hud_stats_.path_nodes.empty()) {
                std::ostringstream path_oss;
                for (std::size_t i = 0; i < hud_stats_.path_nodes.size(); ++i) {
                    path_oss << hud_stats_.path_nodes[i] << (i + 1 < hud_stats_.path_nodes.size() ? " -> " : "");
                }
                ImGui::TextWrapped("Route: %s", path_oss.str().c_str());
            }
        } else if (canvas_.selected_start().has_value() && canvas_.selected_target().has_value()) {
            ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "No path to target.");
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void App::render_help_modal() {
    ImGui::OpenPopup("HelpGuideModal");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(520.0f, 0.0f));

    if (ImGui::BeginPopupModal("HelpGuideModal", &show_help_modal_, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Interactive Dijkstra GUI Guide");
        ImGui::Separator();

        ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.3f, 1.0f), "Canvas Controls:");
        ImGui::BulletText("Double-Click on Canvas: Create a new vertex.");
        ImGui::BulletText("Left-Click & Drag Vertex: Move vertex position.");
        ImGui::BulletText("Shift + Click Vertex: Connect to another vertex.");
        ImGui::BulletText("Left-Click Vertex: Set as Start (1st) or Target (2nd).");
        ImGui::BulletText("Right-Click on Vertex / Edge: Context menu (Set Start/Target, Edit Weight, Delete).");
        ImGui::BulletText("Right/Middle Drag: Pan canvas | Scroll Wheel: Zoom in/out.");

        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.3f, 1.0f), "Color Legend:");
        ImGui::BulletText("[Green] Start vertex.");
        ImGui::BulletText("[Red] Target vertex.");
        ImGui::BulletText("[Orange] Candidate vertex in Priority Queue (Open Set).");
        ImGui::BulletText("[Blue] Settled vertex with optimal distance (Closed Set).");
        ImGui::BulletText("[Gold] Optimal shortest path route.");

        ImGui::Separator();
        if (ImGui::Button("Got it!", ImVec2(120.0f, 0.0f))) {
            show_help_modal_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void App::render_random_modal() {
    ImGui::OpenPopup("RandomGraphModal");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360.0f, 0.0f));

    if (ImGui::BeginPopupModal("RandomGraphModal", &show_random_modal_, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Generate Random 2D Geometric Graph");
        ImGui::Separator();

        ImGui::SliderInt("Vertices", &random_nodes_input_, 3, 30);
        ImGui::SliderInt("Connections", &random_connections_input_, 1, 6);

        ImGui::Separator();
        if (ImGui::Button("Generate", ImVec2(120.0f, 0.0f))) {
            vg_ = Presets::random_geometric(
                static_cast<std::size_t>(random_nodes_input_),
                static_cast<std::size_t>(random_connections_input_),
                550.0f,
                400.0f
            );
            canvas_.center_on_graph(vg_, ImGui::GetIO().DisplaySize);
            if (mode_ == AppMode::INSTANT) solve_instant();
            show_random_modal_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, 0.0f))) {
            show_random_modal_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

} // namespace dijkstra::gui
