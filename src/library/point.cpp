/**
 * @file point.cpp
 * @brief Implementation of Point geometric utilities.
 */

#include "dijkstra/point.h"

#include <cmath>
#include <random>

namespace dijkstra {

double euclidean_distance(const Point &p1, const Point &p2) noexcept {
    return std::hypot(p1.x - p2.x, p1.y - p2.y);
}

std::vector<Point> random_points(
    std::size_t count,
    std::optional<std::uint64_t> seed
) {
    std::mt19937_64 engine;
    if (seed.has_value()) {
        engine.seed(*seed);
    } else {
        std::random_device rd;
        engine.seed(rd());
    }

    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    std::vector<Point> points(count);

    for (std::size_t i = 0; i < count; ++i) {
        points[i] = Point{.x = dist(engine), .y = dist(engine)};
    }

    return points;
}

} // namespace dijkstra
