// March 18, 2023

#pragma once

#include <iostream>
#include <vector>

class Graph {
public:
    Graph(unsigned long size) : size(_size), _size(size), weights(size * size, 0) { }

    inline double getWeight(unsigned long i, unsigned long j) const {
        return weights[i * _size + j];
    }

    inline void setWeight(unsigned long i, unsigned long j, double value) {
        weights[i * _size + j] = weights[i + _size * j] = value;
    }

    const unsigned long & size;

    static Graph sample();
    static Graph random(unsigned long size, unsigned connections);

    friend std::ostream & operator << (std::ostream & os, const Graph & graph);
    friend std::istream & operator >> (std::istream & is, Graph & graph);

private:
    unsigned long _size;
    std::vector<double> weights;

    void resize(unsigned long size);
};
