#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We can of course easily compare all points to all other points, compute
    the areas of the corresponding rectangles, and keep track of the largest
    rectangle. This is straightforward, and at a runtime of around 100us,
    not even that slow. But can we do better?

    One observation is that, since we only care about the largest rectangle,
    we can limit ourselves to a small set of potential corners. For example,
    if we've already found a rectangle with a top left corner at (1,1), there
    is no point anymore in checking rectangles with a top left corner at (2,2),
    since all rectangles with this top left corner will be smaller than the
    largest rectangle with top left at (1,1). We can use this to get a set of
    potential top left corners: first sort the points by X, iterate from left
    to right, and only add points to the list of top left corners if its Y is
    lower than that of any other points in this corner list.

    Using the same approach, we can get potential corner points for the bottom
    left, top right, and bottom right corners. Now we only have to check all
    top left corners against all bottom right corners, and all bottom left
    corners against all top right corners. Since these four sets are much
    smaller than the full set of points, we have to check far fewer areas,
    and the solution becomes significantly faster (~100us to ~30us).
*/

void get_left_corners(const std::vector<Point>& points, std::vector<Point>& tl_corners, std::vector<Point>& bl_corners) {
    long tl_max_y = tl_corners.front().y;
    long bl_min_y = bl_corners.front().y;

    for (size_t index = 0; index < points.size(); ++index) {
        const Point& point = points[index];

        if (point.y < tl_max_y) {
            tl_corners.push_back(point);
            tl_max_y = point.y;
        }

        if (point.y > bl_min_y) {
            bl_corners.push_back(point);
            bl_min_y = point.y;
        }
    }
}

void get_right_corners(const std::vector<Point>& points, std::vector<Point>& tr_corners, std::vector<Point>& br_corners) {
    long tr_max_y = tr_corners.front().y;
    long br_min_y = br_corners.front().y;

    for (int index = points.size() - 1; index >= 0; --index) {
        const Point& point = points[index];

        if (point.y < tr_max_y) {
            tr_corners.push_back(point);
            tr_max_y = point.y;
        }

        if (point.y > br_min_y) {
            br_corners.push_back(point);
            br_min_y = point.y;
        }
    }
}

void find_max_area(const std::vector<Point>& corners_a, const std::vector<Point>& corners_b, long& max_area) {
    for (const Point& a : corners_a) {
        for (const Point& b : corners_b) {
            long dx = std::abs(a.x - b.x) + 1;
            long dy = std::abs(a.y - b.y) + 1;
            max_area = std::max(max_area, dx * dy);
        }
    }
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<Point> points = std::vector<Point>(lines.size());
    parse_points(lines, points);

    std::vector<Point> tl_corners { points.front() };
    std::vector<Point> bl_corners { points.front() };
    std::vector<Point> tr_corners { points.back()  };
    std::vector<Point> br_corners { points.back()  };

    std::ranges::sort(points, compare_points);
    get_left_corners(points, tl_corners, bl_corners);
    get_right_corners(points, tr_corners, br_corners);

    long max_area = 0;

    find_max_area(tl_corners, br_corners, max_area);
    find_max_area(tr_corners, bl_corners, max_area);
    
    return Solution { max_area };
}
