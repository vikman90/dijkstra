// April 9, 2023

#include <gtest/gtest.h>
#include <point.h>

TEST(Point, EdgeCompare) {
    auto e1 = Edge { .distance = 1 };
    auto e2 = Edge { .distance = 2 };

    EXPECT_EQ(EdgeCompare()(e1, e2), false);
}

TEST(Point, RandomPoints) {
    auto points = randomPoints(5);

    EXPECT_EQ(points.size(), 5);

    for (auto i = 0; i < 5; i++) {
        EXPECT_GE(points[i].x, -1);
        EXPECT_LT(points[i].x, +1);
        EXPECT_GE(points[i].y, -1);
        EXPECT_LT(points[i].y, +1);
    }
}
