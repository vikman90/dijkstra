/**
 * @file node.hpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Implementation of class Node
 * @version 0.1
 * @date 2023-04-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <cfloat>
#include <climits>
#include <queue>
#include <vector>

/**
 * @brief Path node
 *
 * It represents each node of a path. Nodes make sense together within a vector.
 *
 */
struct Node {
    Node();                 ///< Constructor
    unsigned long index;    ///< Index of this node in the array
    unsigned long prev;     ///< Index of the previous (parent) node
    bool visited;           ///< Visited flag
    double cost;            ///< Accumulated cost in the path
};

/**
 * @brief Node comparator
 *
 * This class is used by @ref NodeQueue.
 *
 */
struct NodeCompare {
    /**
     * @brief Comparison operator
     *
     * The implementation is oriented so that @ref NodeQueue returns the node
     * with the lowest cost.
     *
     * @param n1 A pointer to a node
     * @param n2 A pointer to another node
     * @retval true @p n1 has a higher cost than @p n2
     * @retval false Otherwise
     */
    bool operator()(const Node * n1, const Node * n2);
};

/// @brief Priority queue of nodes
typedef std::priority_queue<Node *, std::vector<Node *>, NodeCompare> NodeQueue;

/**
 * @brief Print a vector of nodes into the standard input
 *
 * @param nodes A vector of nodes
 * @retval true The graph is connected
 * @retval false The graph is not connected
 */
bool printNodes(const std::vector<Node> & nodes);
