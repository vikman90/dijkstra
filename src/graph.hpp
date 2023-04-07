/**
 * @file graph.hpp
 * @author Vikman Fernandez-Castro (vmfdez90@gmail.com)
 * @brief Declaration of class Graph
 * @version 0.1
 * @date 2023-03-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <iostream>
#include <vector>

/// @brief Weighted graph

class Graph {
public:
    /**
     * @brief Construct a new Graph object
     *
     * @param size Number of nodes
     */
    Graph(unsigned long size) : size(_size), _size(size), weights(size * size, 0) { }

    /**
     * @brief Get the weight of an edge
     *
     * @param i A node index
     * @param j Another node index
     * @return Weight of the connection between nodes @p i and @p j
     * @retval 0 The nodes are not connected
     */
    inline double getWeight(unsigned long i, unsigned long j) const {
        return weights[i * _size + j];
    }

    /**
     * @brief Set the weight of an edge
     *
     * @param i A node index
     * @param j Another node index
     * @param value The weight value
     * @post This operation is symmetric: @c setWeight(i,j) ≡ @c setWeight(j,i)
     */
    inline void setWeight(unsigned long i, unsigned long j, double value) {
        weights[i * _size + j] = weights[i + _size * j] = value;
    }

    /// @brief Number of nodes in the graph
    const unsigned long & size;

    /**
     * @brief Get a sample graph
     *
     * @image html https://upload.wikimedia.org/wikipedia/commons/5/57/Dijkstra_Animation.gif
     *
     * @return A representation of the graph above
     */
    static Graph sample();

    /**
     * @brief Create a random graph
     *
     * @param size Number of nodes
     * @param connections Number of connections per node
     * @return A random connected graph
     */
    static Graph random(unsigned long size, unsigned connections);

    friend std::ostream & operator << (std::ostream & os, const Graph & graph);
    friend std::istream & operator >> (std::istream & is, Graph & graph);

private:

    /// @brief Number of nodes in the graph
    unsigned long _size;

    /// @brief Weights matrix
    std::vector<double> weights;

    /**
     * @brief Resize a graph
     *
     * @param size The new size
     */
    void resize(unsigned long size);
};
