#include "../../src/gui/canvas.h"

#include <gtest/gtest.h>

using namespace dijkstra;
using namespace dijkstra::gui;

TEST(CanvasTest, VisualGraphAddAndRemoveNodes) {
    VisualGraph vg;
    EXPECT_EQ(vg.node_count(), 0);

    NodeId n0 = vg.add_node(100.0f, 150.0f);
    NodeId n1 = vg.add_node(300.0f, 150.0f);
    NodeId n2 = vg.add_node(500.0f, 250.0f);

    EXPECT_EQ(vg.node_count(), 3);
    EXPECT_EQ(n0, 0);
    EXPECT_EQ(n1, 1);
    EXPECT_EQ(n2, 2);

    vg.add_edge(0, 1, 5.0);
    vg.add_edge(1, 2, 4.0);
    EXPECT_EQ(vg.graph().edge_count(), 2);

    // Remove middle node (1)
    vg.remove_node(1);
    EXPECT_EQ(vg.node_count(), 2);
    // Node 2 is remapped to 1, edge 0-1 is deleted, no edge between 0 and new 1
    EXPECT_FALSE(vg.graph().has_edge(0, 1));
}

TEST(CanvasTest, HitTestNode) {
    VisualGraph vg;
    vg.add_node(100.0f, 100.0f);
    vg.add_node(300.0f, 300.0f);

    Canvas canvas;

    // Direct hit on node 0
    auto hit0 = canvas.hit_test_node(vg, ImVec2(105.0f, 98.0f), 24.0f);
    ASSERT_TRUE(hit0.has_value());
    EXPECT_EQ(*hit0, 0);

    // Direct hit on node 1
    auto hit1 = canvas.hit_test_node(vg, ImVec2(295.0f, 305.0f), 24.0f);
    ASSERT_TRUE(hit1.has_value());
    EXPECT_EQ(*hit1, 1);

    // Miss on empty space
    auto miss = canvas.hit_test_node(vg, ImVec2(200.0f, 200.0f), 24.0f);
    EXPECT_FALSE(miss.has_value());
}

TEST(CanvasTest, HitTestEdge) {
    VisualGraph vg;
    vg.add_node(100.0f, 100.0f); // 0
    vg.add_node(300.0f, 100.0f); // 1
    vg.add_edge(0, 1, 10.0);

    Canvas canvas;

    // Hit close to midpoint of edge 0-1 (200, 100)
    auto hit_edge = canvas.hit_test_edge(vg, ImVec2(200.0f, 103.0f), 8.0f);
    ASSERT_TRUE(hit_edge.has_value());
    EXPECT_TRUE((hit_edge->first == 0 && hit_edge->second == 1) ||
                (hit_edge->first == 1 && hit_edge->second == 0));

    // Miss far away
    auto miss_edge = canvas.hit_test_edge(vg, ImVec2(200.0f, 150.0f), 8.0f);
    EXPECT_FALSE(miss_edge.has_value());
}

TEST(CanvasTest, CoordinateTransformations) {
    Canvas canvas;
    ImVec2 origin(50.0f, 50.0f);

    // Test identity transform
    ImVec2 world(150.0f, 200.0f);
    ImVec2 screen = canvas.world_to_screen(world, origin);
    EXPECT_FLOAT_EQ(screen.x, 200.0f);
    EXPECT_FLOAT_EQ(screen.y, 250.0f);

    ImVec2 back_to_world = canvas.screen_to_world(screen, origin);
    EXPECT_FLOAT_EQ(back_to_world.x, world.x);
    EXPECT_FLOAT_EQ(back_to_world.y, world.y);
}
