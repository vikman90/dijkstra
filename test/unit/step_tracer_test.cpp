#include "dijkstra/graph.h"
#include "../../src/gui/step_tracer.h"

#include <gtest/gtest.h>

using namespace dijkstra;
using namespace dijkstra::gui;

TEST(StepTracerTest, WikipediaSampleToTarget) {
    Graph g = Graph::sample();
    auto steps = StepTracer::trace(g, 0, 4);

    ASSERT_FALSE(steps.empty());

    // Step 0 is START
    EXPECT_EQ(steps.front().type, StepType::START);
    EXPECT_EQ(steps.front().current_node, 0);

    // Last step is TARGET_REACHED
    EXPECT_EQ(steps.back().type, StepType::TARGET_REACHED);
    EXPECT_EQ(steps.back().current_node, 4);
    EXPECT_DOUBLE_EQ(steps.back().edge_weight, 20.0);

    // Active path
    std::vector<NodeId> expected_path = {0, 2, 5, 4};
    EXPECT_EQ(steps.back().active_path, expected_path);

    // Check descriptions exist
    for (const auto &step : steps) {
        EXPECT_FALSE(step.description.empty());
    }
}

TEST(StepTracerTest, FullGraphAllPathsTrace) {
    Graph g = Graph::sample();
    auto steps = StepTracer::trace(g, 0, std::nullopt);

    ASSERT_FALSE(steps.empty());
    EXPECT_EQ(steps.front().type, StepType::START);
    EXPECT_EQ(steps.back().type, StepType::COMPLETE);

    // All nodes should eventually be settled
    const auto &final_states = steps.back().node_states;
    for (std::size_t i = 0; i < g.node_count(); ++i) {
        EXPECT_EQ(final_states[i], NodeVisualState::SETTLED);
    }
}

TEST(StepTracerTest, DisconnectedGraphTrace) {
    Graph g(4);
    g.add_edge(0, 1, 3.0);
    g.add_edge(2, 3, 4.0);

    auto steps = StepTracer::trace(g, 0, std::nullopt);
    ASSERT_FALSE(steps.empty());
    EXPECT_EQ(steps.back().type, StepType::COMPLETE);

    const auto &final_states = steps.back().node_states;
    EXPECT_EQ(final_states[0], NodeVisualState::SETTLED);
    EXPECT_EQ(final_states[1], NodeVisualState::SETTLED);
    EXPECT_EQ(final_states[2], NodeVisualState::UNVISITED);
    EXPECT_EQ(final_states[3], NodeVisualState::UNVISITED);
}
