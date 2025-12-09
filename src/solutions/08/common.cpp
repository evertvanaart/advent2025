#include "common.hpp"

#include <charconv>
#include <cmath>
#include <string_view>
#include <string>
#include <vector>

// Parse a string view to a long.

long string_view_to_long(const std::string_view& string_view) {
    long result{};

    std::from_chars(string_view.data(), string_view.data() + string_view.size(), result);

    return result;
}

// Compute the square of the distance between two 3D points.

long compute_distance2(const Point& a, const Point& b) {
    long dx2 = std::pow(a.x - b.x, 2);
    long dy2 = std::pow(a.y - b.y, 2);
    long dz2 = std::pow(a.z - b.z, 2);
    return dx2 + dy2 + dz2;
}

// Parse a 3D point from a line by splitting it at the commas.

void Point::parse(size_t index, const std::string& line) {
    size_t comma_pos1 = line.find(',');
    size_t comma_pos2 = line.find(',', comma_pos1 + 1);

    std::string_view line_sv = std::string_view(line);
    x = string_view_to_long(line_sv.substr(0, comma_pos1));
    y = string_view_to_long(line_sv.substr(comma_pos1 + 1, comma_pos2 - comma_pos1 - 1));
    z = string_view_to_long(line_sv.substr(comma_pos2 + 1));
    i = index;
}

// Initialize an existing Edge object from two 3D points.

void Edge::initialize(const Point& a, const Point& b) {
    distance2 = compute_distance2(a, b);
    index_a = a.i;
    index_b = b.i;
}

// Create a new Edge object from two 3D points.

Edge create_edge(const Point& a, const Point& b) {
    return Edge { compute_distance2(a, b),  a.i, b.i };
}

// Comparator used to sort edges in ascending order of distance squared.

bool compare_edges(const Edge& a, const Edge& b) {
    return a.distance2 < b.distance2;
}

// Parse input lines to a pre-created vector of points.

void parse_points(const std::vector<std::string>& lines, std::vector<Point>& points) {
    for (size_t index = 0; index < lines.size(); ++index) {
        const std::string& line = lines[index];
        points[index].parse(index, line);
    }
}

