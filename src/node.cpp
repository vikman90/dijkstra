// April 1, 2023

#include <iostream>
#include "node.hpp"

using namespace std;

Node::Node() : index(0), prev(ULONG_MAX), visited(false), cost(DBL_MAX) { }

bool NodeCompare::operator()(const Node * n1, const Node * n2) {
    return n1->cost > n2->cost;
}

bool printNodes(const vector<Node> & nodes) {
    bool connected = true;

    for (auto i = 0ul; i < nodes.size(); i++) {
        if (nodes[i].prev == ULONG_MAX) {
            cout << "  → " << i << " [" << ((nodes[i].cost) == 0 ? "0" : "X") << "]\n";

            if (nodes[i].cost != 0) {
                connected = false;
            }
        } else {
            cout << nodes[i].prev << " → " << i << " [" << nodes[i].cost << "]\n";
        }
    }

    return connected;
}
