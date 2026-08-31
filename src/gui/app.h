/**
 * @file app.h
 * @brief Main application state, UI panels, playback controls, and HUD overlay.
 */

#pragma once

#include "canvas.h"
#include "presets.h"
#include "step_tracer.h"

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace dijkstra::gui {

/// @brief Execution mode of the algorithm in the GUI.
enum class AppMode {
    INSTANT, ///< Sub-millisecond calculation with live HUD statistics.
    ANIMATED ///< Interactive step-by-step playback with timeline scrubber.
};

/// @brief Real-time algorithmic performance and topology metrics.
struct HudStatistics {
    double elapsed_us{0.0};
    std::size_t total_nodes{0};
    std::size_t total_edges{0};
    std::size_t settled_nodes{0};
    std::size_t relaxation_count{0};
    std::optional<Weight> total_distance{std::nullopt};
    std::vector<NodeId> path_nodes;
};

/**
 * @brief Top-level GUI Application manager.
 */
class App {
  public:
    App();

    // Main update & render cycle
    void update(float dt);
    void render_ui();

    // Algorithm invocation & playback controls
    void solve_instant();
    void start_animation();
    void play() noexcept;
    void pause() noexcept;
    void toggle_play() noexcept;
    void step_forward();
    void step_backward();
    void reset_playback();
    void set_step_index(int idx);

    // Preset & File Management
    void load_preset(const std::string &name);
    void clear_graph();

    // Headless testing & simulation helpers
    void simulate_double_click(float world_x, float world_y);
    void simulate_connect_nodes(NodeId u, NodeId v, Weight w);
    void simulate_set_selection(std::optional<NodeId> start, std::optional<NodeId> target);

    [[nodiscard]] const VisualGraph &visual_graph() const noexcept {
        return vg_;
    }
    [[nodiscard]] VisualGraph &visual_graph() noexcept {
        return vg_;
    }
    [[nodiscard]] const Canvas &canvas() const noexcept {
        return canvas_;
    }
    [[nodiscard]] Canvas &canvas() noexcept {
        return canvas_;
    }
    [[nodiscard]] const HudStatistics &hud_stats() const noexcept {
        return hud_stats_;
    }
    [[nodiscard]] AppMode mode() const noexcept {
        return mode_;
    }
    [[nodiscard]] bool is_playing() const noexcept {
        return is_playing_;
    }
    [[nodiscard]] int current_step_index() const noexcept {
        return current_step_idx_;
    }
    [[nodiscard]] std::size_t total_steps() const noexcept {
        return animation_steps_.size();
    }
    [[nodiscard]] const std::optional<DijkstraStep> &current_step() const;

  private:
    void render_top_toolbar();
    void render_playback_bar();
    void render_hud_overlay();
    void render_help_modal();
    void render_random_modal();

    VisualGraph vg_;
    Canvas canvas_;

    AppMode mode_{AppMode::INSTANT};
    std::optional<DijkstraResult> instant_result_{std::nullopt};

    std::vector<DijkstraStep> animation_steps_;
    int current_step_idx_{-1};
    mutable std::optional<DijkstraStep> current_step_cached_{std::nullopt};

    bool is_playing_{false};
    float step_timer_{0.0f};
    float step_interval_{0.35f}; // seconds per step

    HudStatistics hud_stats_;

    bool show_help_modal_{false};
    bool show_random_modal_{false};
    int random_nodes_input_{10};
    int random_connections_input_{3};

    char file_path_buffer_[256]{"graph_map.json"};
    std::string status_message_{"Ready. Click on canvas to select Start vertex."};
};

} // namespace dijkstra::gui
