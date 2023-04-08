// April 9, 2023

#include <gtest/gtest.h>
#include <node.h>

TEST(Node, Constructor) {
    auto node = Node();

    EXPECT_EQ(node.index, 0);
    EXPECT_EQ(node.prev, ULONG_MAX);
    EXPECT_EQ(node.visited, false);
    EXPECT_EQ(node.cost, DBL_MAX);
}

TEST(Node, NodeCompare) {
    auto n1 = Node();
    auto n2 = Node();

    n1.cost = 1;
    n2.cost = 2;

    EXPECT_EQ(NodeCompare()(&n1, &n2), false);
}

TEST(Node, CreateNodes) {
    auto nodes = createNodes(5);

    EXPECT_EQ(nodes.size(), 5);

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(nodes[i].index, i);
        EXPECT_EQ(nodes[i].prev, ULONG_MAX);
        EXPECT_EQ(nodes[i].visited, false);
        EXPECT_EQ(nodes[i].cost, DBL_MAX);
    }
}

TEST(Node, PrintNodesConnected) {
    auto nodes = createNodes(2);

    nodes[0].cost = 0;
    nodes[1].prev = 0;
    nodes[1].cost = 3.14;

    testing::internal::CaptureStdout();

    EXPECT_EQ(printNodes(nodes), true);

    auto output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "  → 0 [0]\n0 → 1 [3.14]\n");
}

TEST(Node, PrintNodesDisconnected) {
    auto nodes = createNodes(2);

    nodes[0].cost = 0;

    testing::internal::CaptureStdout();

    EXPECT_EQ(printNodes(nodes), false);

    auto output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "  → 0 [0]\n  → 1 [X]\n");
}
