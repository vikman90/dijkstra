// April 8, 2023

#include <gtest/gtest.h>
#include <sstream>
#include <graph.h>

using namespace std;

class GraphTest {
public:
    GraphTest(Graph & graph) : graph(graph) { }

    void resize(unsigned long size) {
        graph.resize(size);
    }

private:
    Graph & graph;
};

TEST(Graph, Size) {
    auto graph = Graph(5);

    EXPECT_EQ(graph.size, 5);
}

TEST(Graph, GetWeight) {
    auto graph = Graph(5);

    EXPECT_EQ(graph.getWeight(0, 0), 0);
}

TEST(Graph, SetWeight) {
    auto graph = Graph(5);

    graph.setWeight(1, 2, 3.14);

    EXPECT_EQ(graph.getWeight(1, 2), 3.14);
    EXPECT_EQ(graph.getWeight(2, 1), 3.14);
}

TEST(Graph, Sample) {
    auto graph = Graph::sample();

    EXPECT_EQ(graph.getWeight(0, 1), 7);
    EXPECT_EQ(graph.getWeight(0, 2), 9);
    EXPECT_EQ(graph.getWeight(0, 5), 14);
    EXPECT_EQ(graph.getWeight(1, 2), 10);
    EXPECT_EQ(graph.getWeight(1, 3), 15);
    EXPECT_EQ(graph.getWeight(2, 3), 11);
    EXPECT_EQ(graph.getWeight(2, 5), 2);
    EXPECT_EQ(graph.getWeight(3, 4), 6);
    EXPECT_EQ(graph.getWeight(4, 5), 9);
}

TEST(Graph, Random) {
    auto graph = Graph::random(5, 3);

    for (auto i = 0ul; i < graph.size; i++) {
        auto count = 0;

        for (auto j = 0ul; j < graph.size; j++) {
            if (i != j && graph.getWeight(i, j) != 0) {
                count++;
            }
        }

        EXPECT_GE(count, 3);
    }
}

TEST(Graph, Resize) {
    auto graph = Graph(3);
    auto t = GraphTest(graph);

    t.resize(5);

    EXPECT_EQ(graph.size, 5);
}

TEST(Graph, Equal) {
    auto g1 = Graph::sample();
    auto g2 = Graph::sample();

    EXPECT_EQ(g1, g2);
}

TEST(Graph, NotEqual) {
    auto g1 = Graph::sample();
    auto g2 = Graph::sample();

    g2.setWeight(1, 2, 3.14);

    EXPECT_NE(g1, g2);
}

TEST(Graph, Output) {
    auto graph = Graph::sample();
    auto stream = stringstream();

    stream << graph;

    EXPECT_EQ(stream.str(), "7 9 0 0 14\n10 15 0 0\n11 0 2\n6 0\n9\n");
}

TEST(Graph, Input) {
    auto graph = Graph(0);
    auto stream = stringstream("7 9 0 0 14\n10 15 0 0\n11 0 2\n6 0\n9\n");

    stream >> graph;

    EXPECT_EQ(graph, Graph::sample());
}
