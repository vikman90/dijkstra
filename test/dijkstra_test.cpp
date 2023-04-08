// April 8, 2023

#include <gtest/gtest.h>
#include <dijkstra.h>

TEST(Dijkstra, Dijkstra) {
    auto nodes = dijkstra(Graph::sample(), 0);

    EXPECT_EQ(nodes.size(), 6);

    EXPECT_EQ(nodes[0].index, 0);
    EXPECT_EQ(nodes[0].prev, ULONG_MAX);
    EXPECT_EQ(nodes[0].visited, true);
    EXPECT_EQ(nodes[0].cost, 0);

    EXPECT_EQ(nodes[1].index, 1);
    EXPECT_EQ(nodes[1].prev, 0);
    EXPECT_EQ(nodes[1].visited, true);
    EXPECT_EQ(nodes[1].cost, 7);

    EXPECT_EQ(nodes[2].index, 2);
    EXPECT_EQ(nodes[2].prev, 0);
    EXPECT_EQ(nodes[2].visited, true);
    EXPECT_EQ(nodes[2].cost, 9);

    EXPECT_EQ(nodes[3].index, 3);
    EXPECT_EQ(nodes[3].prev, 2);
    EXPECT_EQ(nodes[3].visited, true);
    EXPECT_EQ(nodes[3].cost, 20);

    EXPECT_EQ(nodes[4].index, 4);
    EXPECT_EQ(nodes[4].prev, 5);
    EXPECT_EQ(nodes[4].visited, true);
    EXPECT_EQ(nodes[4].cost, 20);

    EXPECT_EQ(nodes[5].index, 5);
    EXPECT_EQ(nodes[5].prev, 2);
    EXPECT_EQ(nodes[5].visited, true);
    EXPECT_EQ(nodes[5].cost, 11);
}
