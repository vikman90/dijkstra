/**
 * @file dijkstra.h
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Dijkstra function declaration
 * @version 0.1
 * @date 2023-04-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "graph.h"
#include "node.h"

/**
 * @brief Compute the Dijkstra's algorithm
 *
 * @param graph A weighted graph
 * @param begin Index of the starting node
 * @return A vector of nodes having the optimum paths
 */
std::vector<Node> dijkstra(const Graph &graph, unsigned long begin);
