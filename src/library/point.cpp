/**
 * @file point.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Implementation of graph support classes
 * @version 0.1
 * @date 2023-04-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "point.h"
#include <random>

using namespace std;

// Generate a vector of random points

vector<Point> randomPoints(unsigned long size) {
    static auto gen = mt19937(random_device()());
    auto points = vector<Point>(size);
    auto distribution = uniform_real_distribution<>(-1.0, +1.0);

    for (auto i = 0ul; i < size; i++) {
        points[i] = Point{.x = distribution(gen), .y = distribution(gen)};
    }

    return points;
}

// Comparison operator

bool EdgeCompare::operator()(const Edge &e1, const Edge &e2) {
    return e1.distance > e2.distance;
}
