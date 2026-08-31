#include "dijkstra/point.h"

#include <gtest/gtest.h>
#include <cmath>

using namespace dijkstra;

TEST(PointTest, EuclideanDistanceStandard) {
    Point p1{0.0, 0.0};
    Point p2{3.0, 4.0};
    EXPECT_DOUBLE_EQ(euclidean_distance(p1, p2), 5.0);
    EXPECT_DOUBLE_EQ(euclidean_distance(p2, p1), 5.0);
}

TEST(PointTest, EuclideanDistanceZero) {
    Point p1{2.5, -1.8};
    Point p2{2.5, -1.8};
    EXPECT_DOUBLE_EQ(euclidean_distance(p1, p2), 0.0);
}

TEST(PointTest, EuclideanDistanceNegativeCoords) {
    Point p1{-1.0, -1.0};
    Point p2{2.0, 3.0};
    EXPECT_DOUBLE_EQ(euclidean_distance(p1, p2), 5.0);
}

TEST(PointTest, RandomPointsCountAndBounds) {
    const std::size_t count = 50;
    auto points = random_points(count, 42);

    ASSERT_EQ(points.size(), count);
    for (const auto &p : points) {
        EXPECT_GE(p.x, -1.0);
        EXPECT_LE(p.x, 1.0);
        EXPECT_GE(p.y, -1.0);
        EXPECT_LE(p.y, 1.0);
    }
}

TEST(PointTest, RandomPointsDeterministicSeed) {
    auto points1 = random_points(10, 12345);
    auto points2 = random_points(10, 12345);
    EXPECT_EQ(points1, points2);
}
