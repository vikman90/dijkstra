// April 6, 2023

#pragma once

#include <queue>
#include <vector>

struct Point {
    double x;
    double y;
};

struct Edge {
    unsigned long p1;
    unsigned long p2;
    double distance;
};

struct EdgeCompare {
    bool operator()(const Edge & e1, const Edge & e2);
};

typedef std::priority_queue<Edge, std::vector<Edge>, EdgeCompare> EdgeQueue;

std::vector<Point> randomPoints(unsigned long size);
