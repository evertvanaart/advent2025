#include <algorithm>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    For the second part, we do compute and sort all edges. It should be noted
    that we probably didn't have to; in practice, we only use the shortest 5000
    or so edges for the given input, so we could probably speed this up using
    the approach of the first part: initially find only first shortest X edges,
    and increase this X (e.g. doubling it) if this top X is not yet enough to
    connect all points. Still, in the worst case we do need almost all edges
    (e.g. if one point is really far away from all others), and I prefer to
    avoid optimizations that are tailored to a specific input.

    Once we've sorted the edges, we use more or less the same approach as in
    the first part to group the points. The only difference is that we now keep
    track of the number of points that have not yet been assigned to a group,
    as well as the number of unique groups (which goes up whenever we create
    a group, and down when we merge two groups). When we've got one group left
    and no more remaining nodes, we've reached the target state, and we return
    the last edge that we connected.

    There's probably a more efficient algorithm for this, as well as potential
    optimizations to the current approach (e.g. it should be possible to avoid
    the O(N) sweep when merging two groups), but the current solution runs in
    well under 100ms, and I've already spent enough time on this day.
*/

// Create a sorted vector of all edges in the graph.

void initialize_edges(const std::vector<Point>& points, std::vector<Edge>& edges) {
    size_t edge_index = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            edges[edge_index++].initialize(points[i], points[j]);
        }
    }

    std::ranges::sort(edges, compare_edges);
}

// Connect edges and create groups – using the same approach as in the first
// part – until all points are part of one group, and return the last edge.

const Edge& find_last_edge(const std::vector<Edge>& edges, std::vector<int>& point_groups) {
    size_t unassigned_points = point_groups.size();
    size_t unique_group_count = 0;
    int group_counter = 1;

    for (size_t edge_index = 0; ; ++edge_index) {
        const Edge& edge = edges[edge_index];
        size_t index_a = edge.index_a;
        size_t index_b = edge.index_b;
        int group_a = point_groups[index_a];
        int group_b = point_groups[index_b];

        if (group_a == 0 && group_b == 0) {
            // Two points without group; create a new group.
            point_groups[index_a] = group_counter;
            point_groups[index_b] = group_counter++;
            unassigned_points -= 2;
            unique_group_count++;
        } else if (group_a == 0) {
            // Add point A to the existing group of point B.
            point_groups[index_a] = group_b;
            unassigned_points--;
        } else if (group_b == 0) {
            // Add point B to the existing group of point A.
            point_groups[index_b] = group_a;
            unassigned_points--;
        } else if (group_a != group_b) {
            // Two existing different groups; merge them by replacing all
            // instances of the group ID of point B by that of point A. 
            for (size_t index = 0; index < point_groups.size(); ++index) {
                if (point_groups[index] == group_b) {
                    point_groups[index] = group_a;
                }
            }

            unique_group_count--;
        }

        // All points are in a single large group, return the current edge.
        if (unassigned_points == 0 && unique_group_count == 1) {
            return edge;
        }
    }
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<Point> points = std::vector<Point>(lines.size());
    parse_points(lines, points);

    size_t point_count = points.size();
    size_t edge_count = (point_count * (point_count - 1)) / 2;
    std::vector<Edge> edges = std::vector<Edge>(edge_count);
    initialize_edges(points, edges);

    std::vector<int> point_groups = std::vector<int>(points.size(), 0);
    const Edge& last_edge = find_last_edge(edges, point_groups);

    const Point& last_point_a = points[last_edge.index_a];
    const Point& last_point_b = points[last_edge.index_b];

    return Solution { last_point_a.x * last_point_b.x };
}
