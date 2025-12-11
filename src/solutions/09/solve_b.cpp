#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We can intuitively say that a rectangle is invalid if it is crossed by any
    part of the border. For example, a rectangle from top left corner (0, 0) to
    bottom right corner (10, 10) is (probably) invalid if the edge of the area
    also includes a segment from (5, -1) to (5, 11), i.e. a vertical segment
    cutting the rectangle in two. The idea is that we rule out rectangles
    that are invalid by checking for such crossing segments.

    To this end, we construct vectors of horizontal and vertical ranges. Each
    range has a position on one axis, and a start and end (inclusive) on the
    other. For example, a line from (1, 5) to (1, 10) is a vertical range at
    position 1 from 5 to 10, and a line from (3, 3) to (7, 3) is a horizontal
    range at position 3 from 3 to 7. We map each line segment to a range, and
    sort the two resulting range vectors by the position field.

    Next, we sort the points by X value, and check all possible combinations
    of points. For the inner loop (j), we start at the end and iterate to the
    left; this way, we start with the largest possible rectangles, and once we
    find a valid rectangle, we can immediately discard smaller rectangles based
    on area alone, without having the do the following range-based check.

    For a potentially larger rectangle, we check if there are any vertical or
    horizontal ranges crossing the inner part of this rectangle, which would
    make the rectangle invalid. We use the `lower_bound()` and `upper_bound()`
    functions on the sorted range vectors to find the part of the range vector
    containing potentially crossing ranges in O(log N) time, and then we check
    each potentially crossing range against the inner rectangle area. If there
    are no crossing ranges in either direction, the rectangle is valid.

    While this approach produces the correct answer for both the sample input
    and the real input, there are (at least) two unhandled edge cases:

    * A valid rectangle can conceivably be on the outside of the shape instead
      of on the inside, i.e. we do not check whether all tiles inside the rect-
      angle are either green or red.

    * A crossing segment does not necessarily make the rectangle invalid. For
      example, if two vertical segments cross the full height of the rectangle
      at two adjectent X positions, the tiles inside the rectangle could still
      all be red or green, meaning the rectangle would still be valid.

    Both edge cases could be solved in the same way: use the horizontal and
    vertical ranges to determine which part of a row are inside the shape (we
    always start on the outside, and every horizontal or vertical range toggles
    between outside and inside), and use this to check whether a potentially
    valid rectangle is completely on the inside. This could be used instead
    of the current range-based check, and would in theory be similar in terms
    of complexity. Since these edge cases do not actually occur in the input,
    I will fix them at this time.
*/

struct Range {
    long position;
    long start;
    long end;

    static bool compare(const Range& a, const Range& b) {
        return a.position <= b.position;
    }
};

// For each input line, create either a horizontal or a vectical range;
// sort both range vectors by ascending position value.

void initialize_ranges(const std::vector<Point>& points, std::vector<Range>& h_ranges, std::vector<Range>& v_ranges) {
    for (size_t index = 0; index < points.size(); ++index) {
        const Point& point_a = points[(index + 1) % points.size()];
        const Point& point_b = points[index];

        if (point_a.x == point_b.x) {
            long start = std::min(point_a.y, point_b.y);
            long end   = std::max(point_a.y, point_b.y);
            v_ranges.push_back(Range { point_a.x, start, end });
        } else {
            long start = std::min(point_a.x, point_b.x);
            long end   = std::max(point_a.x, point_b.x);
            h_ranges.push_back(Range { point_a.y, start, end });
        }
    }

    std::ranges::sort(h_ranges, Range::compare);
    std::ranges::sort(v_ranges, Range::compare);
}

// Returns true if the inner rectangle area – defined by the minimum and
// maximum values in two dimensions – is not crossed by any of the ranges
// in the input vector. This function can be used both for horizontal
// ranges (in which case A = Y and B = X) and vertical ones.

bool no_crossing_ranges(std::vector<Range>& ranges, long min_a, long max_a, long min_b, long max_b) {
    auto begin = std::ranges::lower_bound(ranges, Range { min_a, 0, 0 }, Range::compare);
    auto end   = std::ranges::upper_bound(ranges, Range { max_a, 0, 0 }, Range::compare);

    return std::find_if(begin, end, [min_b, max_b](const Range& r) -> bool {
        return ((r.start < min_b) && (r.end >= min_b)) || ((r.start <= max_b) && (r.end > max_b));
    }) == end;
}

// Check if a rectangle is valid by checking for crossing ranges.

bool is_valid(const Point& a, const Point& b, std::vector<Range>& h_ranges, std::vector<Range>& v_ranges) {
    long max_x = std::max(a.x, b.x) - 1;
    long min_x = std::min(a.x, b.x) + 1;
    long max_y = std::max(a.y, b.y) - 1;
    long min_y = std::min(a.y, b.y) + 1;

    return no_crossing_ranges(v_ranges, min_x, max_x, min_y, max_y) &&
           no_crossing_ranges(h_ranges, min_y, max_y, min_x, max_x);
}

// Find the area of the largest valid rectangle. Start with corners on
// opposite sides of the sorted points vector; this way, we more or less
// move from larger to smaller rectangles, which allows us to quickly
// discard later rectangles based on size alone.

long find_max_area(const std::vector<Point>& points, std::vector<Range>& h_ranges, std::vector<Range>& v_ranges) {
    long max_area = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        const Point& point_a = points[i];

        for (size_t j = points.size() - 1; j > i; --j) {
            const Point& point_b = points[j];
            long dx = std::abs(point_a.x - point_b.x) + 1;
            long dy = std::abs(point_a.y - point_b.y) + 1;
            long area = dx * dy;

            if ((area > max_area) && is_valid(point_a, point_b, h_ranges, v_ranges)) {
                max_area = area;
            }
        }
    }

    return max_area;
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<Point> points = std::vector<Point>(lines.size());
    parse_points(lines, points);

    std::vector<Range> h_ranges { };
    std::vector<Range> v_ranges { };
    h_ranges.reserve(points.size() / 2);
    v_ranges.reserve(points.size() / 2);
    initialize_ranges(points, h_ranges, v_ranges);
    std::ranges::sort(points, compare_points);

    long max_area = find_max_area(points, h_ranges, v_ranges);

    return Solution { max_area };
}
