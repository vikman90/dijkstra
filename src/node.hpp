// April 1, 2023

#pragma once

#include <cfloat>
#include <climits>
#include <queue>
#include <vector>

struct Node {
    Node();
    unsigned long index;
    unsigned long prev;
    bool visited;
    double cost;
};

struct NodeCompare {
    bool operator()(const Node * n1, const Node * n2);
};

typedef std::priority_queue<Node *, std::vector<Node *>, NodeCompare> NodeQueue;

bool printNodes(const std::vector<Node> & nodes);
