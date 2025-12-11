#pragma once

#include <string>
#include <vector>

struct Point {
    long x;
    long y;

    void parse(const std::string& line);
};

void parse_points(const std::vector<std::string>& lines, std::vector<Point>& points);

bool compare_points(const Point& a, const Point& b);
