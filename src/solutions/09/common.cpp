#include "common.hpp"

#include <charconv>
#include <string_view>

// Parse a string view to a long.

long string_view_to_long(const std::string_view& string_view) {
    long result{};

    std::from_chars(string_view.data(), string_view.data() + string_view.size(), result);

    return result;
}

// Parse a point from an input line by splitting at the comma.

void Point::parse(const std::string& line) {
    std::string_view sv = std::string_view(line);
    size_t comma_pos = line.find(',');
    x = string_view_to_long(sv.substr(0, comma_pos));
    y = string_view_to_long(sv.substr(comma_pos + 1));
}

// Parse all points in the input lines.

void parse_points(const std::vector<std::string>& lines, std::vector<Point>& points) {
    for (size_t index = 0; index < lines.size(); ++index) {
        points[index].parse(lines[index]);
    }
}

// Used to sort points by X value.

bool compare_points(const Point& a, const Point& b) {
    return a.x <= b.x;
}
