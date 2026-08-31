/**
 * @file presets.h
 * @brief Preset graph topologies and JSON storage for the Dijkstra GUI.
 */

#pragma once

#include "canvas.h"

#include <optional>
#include <string>

namespace dijkstra::gui {

/**
 * @brief Factory for canonical educational preset graphs.
 */
class Presets {
  public:
    /// @brief Canonical 6-node Wikipedia sample graph arranged aesthetically.
    [[nodiscard]] static VisualGraph wikipedia_sample();

    /// @brief Ring network topology with bidirectional cycle edges.
    [[nodiscard]] static VisualGraph ring_graph(std::size_t nodes = 8, float radius = 180.0f);

    /// @brief Star network with a central hub node.
    [[nodiscard]] static VisualGraph star_graph(std::size_t leaves = 7, float radius = 180.0f);

    /// @brief 2D Grid maze topology with random edge removals.
    [[nodiscard]] static VisualGraph grid_maze(std::size_t rows = 4, std::size_t cols = 5, float spacing = 90.0f);

    /// @brief Random 2D geometric graph within a bounded canvas box.
    [[nodiscard]] static VisualGraph random_geometric(
        std::size_t nodes = 12,
        std::size_t connections = 3,
        float width = 600.0f,
        float height = 450.0f,
        std::optional<std::uint64_t> seed = std::nullopt
    );

    /// @brief Save graph topology and 2D positions to a JSON file.
    static bool save_to_json(const VisualGraph &vg, const std::string &filepath, std::string &error_msg);

    /// @brief Load graph topology and 2D positions from a JSON file.
    static bool load_from_json(VisualGraph &vg, const std::string &filepath, std::string &error_msg);
};

} // namespace dijkstra::gui
