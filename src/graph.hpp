// March 18, 2023

#pragma once

#include <vector>

class Graph {
public:
    Graph(unsigned long size);

    unsigned long size() const;
    double getWeight(unsigned long i, unsigned long j) const;
    void setWeight(unsigned long i, unsigned long j, double value);

    static Graph sample();
    static Graph random(unsigned long size, unsigned connections);

private:
    std::vector<std::vector<double>> weights;
};
