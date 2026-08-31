#include "../../src/gui/app.h"
#include "../../src/gui/presets.h"

#include <gtest/gtest.h>
#include <cstdio>

using namespace dijkstra;
using namespace dijkstra::gui;

TEST(GuiAppTest, InitializationAndDefaultWikipediaSolve) {
    App app;
    EXPECT_EQ(app.visual_graph().node_count(), 6);
    EXPECT_EQ(app.mode(), AppMode::INSTANT);

    const auto &stats = app.hud_stats();
    EXPECT_EQ(stats.total_nodes, 6);
    EXPECT_EQ(stats.total_edges, 9);
    ASSERT_TRUE(stats.total_distance.has_value());
    EXPECT_DOUBLE_EQ(*stats.total_distance, 20.0); // 0 to 4 distance in Wikipedia sample
    EXPECT_EQ(stats.path_nodes, (std::vector<NodeId>{0, 2, 5, 4}));
}

TEST(GuiAppTest, PlaybackControls) {
    App app;
    app.start_animation();

    EXPECT_EQ(app.mode(), AppMode::ANIMATED);
    EXPECT_GT(app.total_steps(), 0);
    EXPECT_EQ(app.current_step_index(), 0);
    EXPECT_TRUE(app.is_playing());

    app.pause();
    EXPECT_FALSE(app.is_playing());

    app.step_forward();
    EXPECT_EQ(app.current_step_index(), 1);

    app.step_backward();
    EXPECT_EQ(app.current_step_index(), 0);

    app.reset_playback();
    EXPECT_EQ(app.current_step_index(), 0);
    EXPECT_FALSE(app.is_playing());
}

TEST(GuiAppTest, PresetsLoading) {
    App app;

    app.load_preset("ring");
    EXPECT_EQ(app.visual_graph().node_count(), 8);

    app.load_preset("star");
    EXPECT_EQ(app.visual_graph().node_count(), 8); // Hub + 7 leaves

    app.load_preset("grid");
    EXPECT_EQ(app.visual_graph().node_count(), 20); // 4 rows x 5 cols
}

TEST(GuiAppTest, HeadlessSimulationWorkflow) {
    App app;
    app.clear_graph();
    EXPECT_EQ(app.visual_graph().node_count(), 0);

    // 1. Add 3 vertices
    app.simulate_double_click(100.0f, 100.0f); // 0
    app.simulate_double_click(200.0f, 100.0f); // 1
    app.simulate_double_click(300.0f, 100.0f); // 2
    EXPECT_EQ(app.visual_graph().node_count(), 3);

    // 2. Connect 0-1 and 1-2
    app.simulate_connect_nodes(0, 1, 4.5);
    app.simulate_connect_nodes(1, 2, 2.5);
    EXPECT_EQ(app.visual_graph().graph().edge_count(), 2);

    // 3. Set Start=0, Target=2
    app.simulate_set_selection(0, 2);

    // 4. Verify HUD results
    const auto &stats = app.hud_stats();
    ASSERT_TRUE(stats.total_distance.has_value());
    EXPECT_DOUBLE_EQ(*stats.total_distance, 7.0);
    EXPECT_EQ(stats.path_nodes, (std::vector<NodeId>{0, 1, 2}));
}

TEST(GuiAppTest, JsonMapSaveAndLoad) {
    VisualGraph vg;
    vg.add_node(100.0f, 150.0f);
    vg.add_node(250.0f, 300.0f);
    vg.add_edge(0, 1, 8.25);

    std::string test_file = "test_map_temp.json";
    std::string err;

    EXPECT_TRUE(Presets::save_to_json(vg, test_file, err));

    VisualGraph loaded;
    EXPECT_TRUE(Presets::load_from_json(loaded, test_file, err));

    EXPECT_EQ(loaded.node_count(), 2);
    EXPECT_EQ(loaded.graph().edge_count(), 1);
    EXPECT_DOUBLE_EQ(loaded.graph().get_weight(0, 1), 8.25);
    EXPECT_FLOAT_EQ(static_cast<float>(loaded.positions()[0].x), 100.0f);

    std::remove(test_file.c_str());
}
