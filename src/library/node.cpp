/**
 * @file node.cpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Declaration of class Node
 * @version 0.1
 * @date 2023-04-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "node.h"
#include <iostream>

using namespace std;

// Constructor

Node::Node() : index(0), prev(ULONG_MAX), visited(false), cost(DBL_MAX) {}

// Comparison operator

bool NodeCompare::operator()(const Node *n1, const Node *n2) {
    return n1->cost > n2->cost;
}

// Create an array of nodes

std::vector<Node> createNodes(unsigned long size) {
    auto nodes = vector<Node>(size);

    for (auto i = 0ul; i < size; i++) {
        nodes[i].index = i;
    }

    return nodes;
}

// Print a vector of nodes into the standard input

bool printNodes(const std::vector<Node> &nodes) {
    bool connected = true;

    for (auto i = 0ul; i < nodes.size(); i++) {
        if (nodes[i].prev == ULONG_MAX) {
            cout << "  → " << i << " [" << ((nodes[i].cost) == 0 ? "0" : "X")
                 << "]\n";

            if (nodes[i].cost != 0) {
                connected = false;
            }
        } else {
            cout << nodes[i].prev << " → " << i << " [" << nodes[i].cost
                 << "]\n";
        }
    }

    return connected;
}
