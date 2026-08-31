#include "dijkstra/dijkstra.h"
#include "dijkstra/exceptions.h"

#include <gtest/gtest.h>

using namespace dijkstra;

TEST(DijkstraTest, SampleGraphAllPaths) {
    Graph g = Graph::sample();
    auto result = shortest_paths(g, 0);

    EXPECT_EQ(result.source(), 0);
    EXPECT_FALSE(result.target().has_value());
    EXPECT_TRUE(result.is_connected());

    // Distances
    EXPECT_DOUBLE_EQ(*result.distance_to(0), 0.0);
    EXPECT_DOUBLE_EQ(*result.distance_to(1), 7.0);
    EXPECT_DOUBLE_EQ(*result.distance_to(2), 9.0);
    EXPECT_DOUBLE_EQ(*result.distance_to(3), 20.0);
    EXPECT_DOUBLE_EQ(*result.distance_to(4), 20.0);
    EXPECT_DOUBLE_EQ(*result.distance_to(5), 11.0);

    // Paths
    EXPECT_EQ(*result.path_to(0), (std::vector<NodeId>{0}));
    EXPECT_EQ(*result.path_to(1), (std::vector<NodeId>{0, 1}));
    EXPECT_EQ(*result.path_to(2), (std::vector<NodeId>{0, 2}));
    EXPECT_EQ(*result.path_to(3), (std::vector<NodeId>{0, 2, 3}));
    EXPECT_EQ(*result.path_to(4), (std::vector<NodeId>{0, 2, 5, 4}));
    EXPECT_EQ(*result.path_to(5), (std::vector<NodeId>{0, 2, 5}));
}

TEST(DijkstraTest, TargetSpecificEarlyExit) {
    Graph g = Graph::sample();
    auto result_full = shortest_paths(g, 0);
    auto result_target = shortest_path(g, 0, 1);

    EXPECT_TRUE(result_target.target().has_value());
    EXPECT_EQ(*result_target.target(), 1);
    EXPECT_DOUBLE_EQ(*result_target.distance_to(1), 7.0);
    EXPECT_EQ(*result_target.path_to(1), (std::vector<NodeId>{0, 1}));

    // Because 1 is settled very early, visited count should be <= full graph settlement
    EXPECT_LE(result_target.visited_count(), result_full.visited_count());
}

TEST(DijkstraTest, DisconnectedGraph) {
    Graph g(4);
    g.add_edge(0, 1, 5.0);
    g.add_edge(2, 3, 2.0);

    auto result = shortest_paths(g, 0);

    EXPECT_FALSE(result.is_connected());
    EXPECT_TRUE(result.has_path_to(1));
    EXPECT_FALSE(result.has_path_to(2));
    EXPECT_FALSE(result.has_path_to(3));

    EXPECT_DOUBLE_EQ(*result.distance_to(1), 5.0);
    EXPECT_EQ(result.distance_to(2), std::nullopt);
    EXPECT_EQ(result.path_to(2), std::nullopt);
}

TEST(DijkstraTest, SingleNodeGraph) {
    Graph g(1);
    auto result = shortest_paths(g, 0);

    EXPECT_TRUE(result.is_connected());
    EXPECT_DOUBLE_EQ(*result.distance_to(0), 0.0);
    EXPECT_EQ(*result.path_to(0), (std::vector<NodeId>{0}));
}

TEST(DijkstraTest, ZeroWeightEdge) {
    Graph g(3);
    g.add_edge(0, 1, 0.0); // Will be ignored / 0
    g.add_edge(1, 2, 4.0);

    // Explicitly add edge with positive weight
    g.add_edge(0, 1, 0.01);
    auto result = shortest_paths(g, 0);
    EXPECT_DOUBLE_EQ(*result.distance_to(1), 0.01);
    EXPECT_DOUBLE_EQ(*result.distance_to(2), 4.01);
}

TEST(DijkstraTest, DirectedGraphShortestPath) {
    Graph g(3, true);
    g.add_edge(0, 1, 2.0);
    g.add_edge(1, 2, 3.0);
    // No path from 2 to 0

    auto r0 = shortest_paths(g, 0);
    EXPECT_DOUBLE_EQ(*r0.distance_to(2), 5.0);

    auto r2 = shortest_paths(g, 2);
    EXPECT_FALSE(r2.has_path_to(0));
}

TEST(DijkstraTest, OutOfBoundsExceptions) {
    Graph g = Graph::sample();
    EXPECT_THROW((void)shortest_paths(g, 10), InvalidNodeException);
    EXPECT_THROW((void)shortest_path(g, 0, 10), InvalidNodeException);
    EXPECT_THROW((void)shortest_path(g, 10, 0), InvalidNodeException);
}
