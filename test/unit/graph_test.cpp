#include "dijkstra/graph.h"
#include "dijkstra/exceptions.h"

#include <gtest/gtest.h>

using namespace dijkstra;

TEST(GraphTest, DefaultConstructor) {
    Graph g;
    EXPECT_EQ(g.node_count(), 0);
    EXPECT_EQ(g.edge_count(), 0);
    EXPECT_FALSE(g.is_directed());
}

TEST(GraphTest, SizedConstructor) {
    Graph g(5);
    EXPECT_EQ(g.node_count(), 5);
    EXPECT_EQ(g.edge_count(), 0);
    EXPECT_FALSE(g.is_directed());
}

TEST(GraphTest, AddAndGetWeightUndirected) {
    Graph g(4, false);
    g.add_edge(0, 1, 3.5);

    EXPECT_TRUE(g.has_edge(0, 1));
    EXPECT_TRUE(g.has_edge(1, 0));
    EXPECT_DOUBLE_EQ(g.get_weight(0, 1), 3.5);
    EXPECT_DOUBLE_EQ(g.get_weight(1, 0), 3.5);
    EXPECT_EQ(g.edge_count(), 1);
}

TEST(GraphTest, AddAndGetWeightDirected) {
    Graph g(4, true);
    g.add_edge(0, 1, 3.5);

    EXPECT_TRUE(g.has_edge(0, 1));
    EXPECT_FALSE(g.has_edge(1, 0));
    EXPECT_DOUBLE_EQ(g.get_weight(0, 1), 3.5);
    EXPECT_DOUBLE_EQ(g.get_weight(1, 0), 0.0);
    EXPECT_EQ(g.edge_count(), 1);
}

TEST(GraphTest, UpdateEdgeWeight) {
    Graph g(3);
    g.add_edge(0, 1, 2.0);
    EXPECT_DOUBLE_EQ(g.get_weight(0, 1), 2.0);
    EXPECT_EQ(g.edge_count(), 1);

    g.add_edge(0, 1, 5.0);
    EXPECT_DOUBLE_EQ(g.get_weight(0, 1), 5.0);
    EXPECT_EQ(g.edge_count(), 1);
}

TEST(GraphTest, RemoveEdgeWithZeroWeight) {
    Graph g(3);
    g.add_edge(0, 1, 2.0);
    EXPECT_TRUE(g.has_edge(0, 1));

    g.add_edge(0, 1, 0.0);
    EXPECT_FALSE(g.has_edge(0, 1));
    EXPECT_EQ(g.edge_count(), 0);
}

TEST(GraphTest, InvalidNodeExceptions) {
    Graph g(3);
    EXPECT_THROW(g.add_edge(0, 5, 1.0), InvalidNodeException);
    EXPECT_THROW(g.add_edge(5, 0, 1.0), InvalidNodeException);
    EXPECT_THROW((void)g.get_weight(0, 5), InvalidNodeException);
    EXPECT_THROW((void)g.neighbors(5), InvalidNodeException);
}

TEST(GraphTest, NegativeWeightException) {
    Graph g(3);
    EXPECT_THROW(g.add_edge(0, 1, -2.5), NegativeWeightException);
}

TEST(GraphTest, NeighborsQuery) {
    Graph g(4);
    g.add_edge(0, 1, 1.0);
    g.add_edge(0, 2, 2.0);

    const auto &n0 = g.neighbors(0);
    EXPECT_EQ(n0.size(), 2);

    const auto &n1 = g.neighbors(1);
    EXPECT_EQ(n1.size(), 1);
    EXPECT_EQ(n1[0].to, 0);
}

TEST(GraphTest, Resize) {
    Graph g(3);
    g.add_edge(0, 1, 2.0);
    g.add_edge(1, 2, 4.0);

    g.resize(5);
    EXPECT_EQ(g.node_count(), 5);
    EXPECT_TRUE(g.has_edge(0, 1));
    EXPECT_TRUE(g.has_edge(1, 2));

    g.resize(2);
    EXPECT_EQ(g.node_count(), 2);
    EXPECT_TRUE(g.has_edge(0, 1));
    EXPECT_THROW((void)g.get_weight(1, 2), InvalidNodeException);
}

TEST(GraphTest, Clear) {
    Graph g = Graph::sample();
    EXPECT_GT(g.node_count(), 0);
    EXPECT_GT(g.edge_count(), 0);

    g.clear();
    EXPECT_EQ(g.node_count(), 0);
    EXPECT_EQ(g.edge_count(), 0);
}

TEST(GraphTest, EqualityOperators) {
    Graph g1 = Graph::sample();
    Graph g2 = Graph::sample();
    EXPECT_EQ(g1, g2);

    g2.add_edge(0, 1, 99.0);
    EXPECT_NE(g1, g2);
}

TEST(GraphTest, SampleGraphStructure) {
    Graph g = Graph::sample();
    EXPECT_EQ(g.node_count(), 6);
    EXPECT_DOUBLE_EQ(g.get_weight(0, 1), 7.0);
    EXPECT_DOUBLE_EQ(g.get_weight(0, 2), 9.0);
    EXPECT_DOUBLE_EQ(g.get_weight(0, 5), 14.0);
    EXPECT_DOUBLE_EQ(g.get_weight(1, 2), 10.0);
    EXPECT_DOUBLE_EQ(g.get_weight(1, 3), 15.0);
    EXPECT_DOUBLE_EQ(g.get_weight(2, 3), 11.0);
    EXPECT_DOUBLE_EQ(g.get_weight(2, 5), 2.0);
    EXPECT_DOUBLE_EQ(g.get_weight(3, 4), 6.0);
    EXPECT_DOUBLE_EQ(g.get_weight(4, 5), 9.0);
}

TEST(GraphTest, IsConnected) {
    Graph g_empty(0);
    EXPECT_TRUE(g_empty.is_connected());

    Graph g_single(1);
    EXPECT_TRUE(g_single.is_connected());

    Graph g_sample = Graph::sample();
    EXPECT_TRUE(g_sample.is_connected());

    Graph g_disconnected(4);
    g_disconnected.add_edge(0, 1, 1.0);
    g_disconnected.add_edge(2, 3, 1.0);
    EXPECT_FALSE(g_disconnected.is_connected());
}

TEST(GraphTest, RandomGeometricDeterministicAndConnected) {
    Graph g1 = Graph::random_geometric(10, 3, 100);
    Graph g2 = Graph::random_geometric(10, 3, 100);
    EXPECT_EQ(g1, g2);
    EXPECT_TRUE(g1.is_connected());

    // Test with multiple sizes and random seeds
    for (std::size_t n = 2; n <= 30; n += 5) {
        Graph g = Graph::random_geometric(n, 1, n * 7);
        EXPECT_TRUE(g.is_connected());
    }
}

TEST(GraphTest, RandomErdosRenyiDeterministic) {
    Graph g1 = Graph::random_erdos_renyi(10, 0.4, 1.0, 5.0, 200);
    Graph g2 = Graph::random_erdos_renyi(10, 0.4, 1.0, 5.0, 200);
    EXPECT_EQ(g1, g2);
}

TEST(GraphTest, ToDotFormat) {
    Graph g(2);
    g.add_edge(0, 1, 4.2);
    std::string dot = g.to_dot("TestGraph");
    EXPECT_NE(dot.find("graph TestGraph {"), std::string::npos);
    EXPECT_NE(dot.find("0 -- 1 [label=\"4.2\"];"), std::string::npos);
}
