// April 6, 2023

#include <random>
#include "point.hpp"

using namespace std;

vector<Point> randomPoints(unsigned long size) {
    static auto gen = mt19937(random_device()());
    auto points = vector<Point>(size);
    auto distribution = uniform_real_distribution<>(-1.0, +1.0);

    for (auto i = 0ul; i < size; i++) {
        points[i] = Point { .x = distribution(gen), .y = distribution(gen) };
    }

    return points;
}

bool EdgeCompare::operator()(const Edge & e1, const Edge & e2) {
    return e1.distance > e2.distance;
}
