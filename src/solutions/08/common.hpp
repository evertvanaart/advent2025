#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Point {
    size_t i;
    long x;
    long y;
    long z;

    void parse(size_t index, const std::string& line);
};

struct Edge {
    long distance2;
    size_t index_a;
    size_t index_b;

    void initialize(const Point& a, const Point& b);
};

Edge create_edge(const Point& a, const Point& b);

bool compare_edges(const Edge& a, const Edge& b);

void parse_points(const std::vector<std::string>& lines, std::vector<Point>& points);
