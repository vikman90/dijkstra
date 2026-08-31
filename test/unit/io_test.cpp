#include "dijkstra/dijkstra.h"
#include "dijkstra/exceptions.h"
#include "dijkstra/graph.h"

#include <gtest/gtest.h>
#include <sstream>

using namespace dijkstra;

TEST(IoTest, StreamOutputAndInputRoundtrip) {
    Graph original = Graph::sample();
    std::stringstream ss;
    ss << original;

    Graph parsed;
    ss >> parsed;

    EXPECT_EQ(original, parsed);
}

TEST(IoTest, MatrixFormatStreamOutput) {
    Graph g = Graph::sample();
    std::stringstream ss;
    ss << g;

    EXPECT_EQ(ss.str(), "7 9 0 0 14\n10 15 0 0\n11 0 2\n6 0\n9\n");
}

TEST(IoTest, MatrixFormatStreamInput) {
    std::stringstream ss("7 9 0 0 14\n10 15 0 0\n11 0 2\n6 0\n9\n");
    Graph g;
    ss >> g;

    EXPECT_EQ(g, Graph::sample());
}

TEST(IoTest, MalformedStreamInputThrowsException) {
    std::stringstream ss("7 9 0 0 14\n10 INVALID_TOKEN 0 0\n");
    Graph g;
    EXPECT_THROW(ss >> g, GraphParseException);
}

TEST(IoTest, PrintSummaryConnected) {
    Graph g(2);
    g.add_edge(0, 1, 3.14);

    auto result = shortest_paths(g, 0);
    std::stringstream ss;
    result.print_summary(ss);

    EXPECT_EQ(ss.str(), "  → 0 [0]\n0 → 1 [3.14]\n");
}

TEST(IoTest, PrintSummaryDisconnected) {
    Graph g(2);
    auto result = shortest_paths(g, 0);
    std::stringstream ss;
    result.print_summary(ss);

    EXPECT_EQ(ss.str(), "  → 0 [0]\n  → 1 [X]\n");
}
