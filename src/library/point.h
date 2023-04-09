/**
 * @file point.h
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Declaration of graph support classes
 * @version 0.1
 * @date 2023-04-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <queue>
#include <vector>

/// @brief Two-dimensional point description
struct Point {
    double x; ///< X axis coordinate
    double y; ///< Y axis coordinate
};

/// @brief Edge between two points
struct Edge {
    unsigned long p1; ///< Index of a point
    unsigned long p2; ///< Index of another point
    double distance;  ///< Distance between the points
};

/**
 * @brief Edge comparator
 *
 * This class is used by @ref EdgeQueue.
 *
 */
struct EdgeCompare {
    /**
     * @brief Comparison operator
     *
     * The implementation is oriented so that @ref EdgeQueue returns the edge
     * with the lowest distance.
     *
     * @param e1 A reference to a edge
     * @param e2 A reference to another edge
     * @retval true @p e1 has a higher distance than @p e2
     * @retval false Otherwise
     */
    bool operator()(const Edge &e1, const Edge &e2);
};

/// @brief Priority queue of edges
typedef std::priority_queue<Edge, std::vector<Edge>, EdgeCompare> EdgeQueue;

/**
 * @brief Generate a vector of random points
 *
 * Coordinates are floating point numbers between -1 and +1 generated randomly
 * according to a uniform distribution.
 *
 * @param size Number of points
 * @return A vector of random points
 */
std::vector<Point> randomPoints(unsigned long size);
