/**
 * @file point.h
 * @brief 2D Point structure and geometric utility functions.
 */

#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace dijkstra {

/**
 * @brief Two-dimensional Cartesian point.
 */
struct Point {
    double x{0.0};
    double y{0.0};

    [[nodiscard]] auto operator<=>(const Point &) const = default;
};

/**
 * @brief Calculate the Euclidean distance between two 2D points.
 *
 * @param p1 First point.
 * @param p2 Second point.
 * @return Euclidean distance $\sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}$.
 */
[[nodiscard]] double euclidean_distance(const Point &p1, const Point &p2) noexcept;

/**
 * @brief Generate a vector of random points uniformly distributed in $[-1.0, 1.0] \times [-1.0, 1.0]$.
 *
 * @param count Number of points to generate.
 * @param seed Optional random seed for deterministic generation.
 * @return Vector of generated 2D points.
 */
[[nodiscard]] std::vector<Point> random_points(
    std::size_t count,
    std::optional<std::uint64_t> seed = std::nullopt
);

} // namespace dijkstra
