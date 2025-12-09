#include <algorithm>
#include <cstddef>
#include <functional>
#include <ranges>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

const size_t NR_CONNECTIONS_SAMPLE = 10;
const size_t NR_CONNECTIONS_INPUT  = 1000;

/*
    The straightforward approach is as follows:
    - Parse the input lines to a vector of 3D points.
    - For each combination of points, compute the length of the edge.
    - Sort the resulting vector of edges by distance in ascending order.
    - Connect the first 1000 edges, keeping of track of connected groups.
    - Find the sizes of the three largest groups.

    One minor optimization is that we can order the edges by the square of
    their distance instead of their actual distance, which removes one square
    root calculation per edge, and allows us to use integer types everywhere.

    When connecting edges, we keep track of the group ID of each point in a
    separate integer vector, with zero meaning that the point is not yet part
    of a group. If the next shortest edge connects two points without a group,
    we create a new group (by incrementing the group ID counter), and assign
    both points to this new group. If only one point is part of a group, the
    lone point becomes part of this group. If both points are in different
    groups, we need to merge these groups, which we do by replacing all
    instances of one group ID in the group ID vector with the other ID.

    Since group IDs are all integers, we can easily compute their sizes using
    another integer vector, one for each group ID. For each non-zero group ID
    in the group ID vector, we increment the corresponding element of the group
    count vector. After this, the only remaining step is to sort the group size
    vector, and compute the product of the three largest group sizes.

    Even though this is a O(N^2) algorithm, it is surprisingly fast, running in
    approximately 35ms on a single core. Still, we can do better. Most of this
    35ms is spent sorting the O(N^2) vector (containing almost half a million
    entries), but we don't actually have to create such a large vector.
    
    Instead, we perform a sort after the first 1000 edges, and then reject any
    new edge longer than the longest edge in this list, on the logic that we're
    only interest in the top 1000 edges. Edge shorter than the current maximum
    do get added to the list, and whenever this list reaches 1000 + X entries
    (where currently, X = 100), we sort it again and drop the last X entries.
    In this way, we only ever have to sort a relatively small list, and the
    vast majority of edges will be immediately rejected.

    Using this optimization, running time reduces from 35ms to less than 4ms.
 */

// Find the top 1000 shortest edges. Start by adding the first 1000 edges to
// the list, then sort it and store the maximum distance (squared to avoid
// square root calculations). Only add the remaining edges to the list if
// they are shorter than this maximum. After every 100 new edges, re-sort
// the list and reduce it to the first 1000 entries again.

void initialize_edges(const std::vector<Point>& points, std::vector<Edge>& edges, size_t max_connections) {
    edges.reserve(max_connections);

    size_t sort_margin = max_connections / 10;
    long max_distance2 = 0;

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            Edge edge = create_edge(points[i], points[j]);

            if (edges.size() < max_connections) {
                edges.push_back(edge);

                if (edges.size() == max_connections) {
                    std::ranges::sort(edges, compare_edges);
                    max_distance2 = edges[max_connections - 1].distance2;
                }
            } else if (edge.distance2 < max_distance2) {
                edges.push_back(edge);

                if (edges.size() == max_connections + sort_margin) {
                    std::ranges::sort(edges, compare_edges);
                    max_distance2 = edges[max_connections - 1].distance2;
                    edges.resize(max_connections);
                }
            }
        }
    }

    std::ranges::sort(edges, compare_edges);
}

// Assign group IDs to points by connecting edges. We use an integer vector to
// track the current group ID of each point, with zero meaning that the point
// has not yet been assigned to a group. For each edge, we either create a new
// group, add a point to an existing group, or merge two exsting groups.

int find_point_groups(const std::vector<Edge>& edges, std::vector<int>& point_groups, size_t max_connections) {
    int group_counter = 1;

    for (size_t connection_count = 0; connection_count < max_connections; ++connection_count) {
        const Edge& edge = edges[connection_count];
        size_t index_a = edge.index_a;
        size_t index_b = edge.index_b;
        int group_a = point_groups[index_a];
        int group_b = point_groups[index_b];

        if (group_a == 0 && group_b == 0) {
            // Two points without group; create a new group.
            point_groups[index_a] = group_counter;
            point_groups[index_b] = group_counter++;
        } else if (group_a == 0) {
            // Add point A to the existing group of point B.
            point_groups[index_a] = group_b;
        } else if (group_b == 0) {
            // Add point B to the existing group of point A.
            point_groups[index_b] = group_a;
        } else if (group_a != group_b) {
            // Two existing different groups; merge them by replacing all
            // instances of the group ID of point B by that of point A. 
            for (size_t index = 0; index < point_groups.size(); ++index) {
                if (point_groups[index] == group_b) {
                    point_groups[index] = group_a;
                }
            }
        }
    }

    return group_counter - 1;
}

// From the point group vector, containing one group ID per input point,
// populate a group size vector, containing the size of each group. Group
// IDs start at one (zero means no group), so we have to subtract one
// before we can use the group ID as a group size vector index.

void find_group_sizes(const std::vector<int>& point_groups, std::vector<int>& group_sizes) {
    auto valid_group_ids = point_groups | std::views::filter([](int group_id) -> bool { return group_id > 0; });
    std::ranges::for_each(valid_group_ids, [&group_sizes](int group_id) { group_sizes[group_id - 1]++; });
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    // The number of connections depends on whether we're processing the sample or the real input.
    size_t max_connections = (input_name == "sample") ? NR_CONNECTIONS_SAMPLE : NR_CONNECTIONS_INPUT;

    // Step 1: Parse all lines to points.
    std::vector<Point> points = std::vector<Point>(lines.size());
    parse_points(lines, points);

    // Step 2: Find the 1000 shortest edges.
    std::vector<Edge> edges = std::vector<Edge>();
    initialize_edges(points, edges, max_connections);

    // Step 3: Connect the 1000 shortest edges, labeling points with group ID.
    std::vector<int> point_groups = std::vector<int>(points.size(), 0);
    int group_count = find_point_groups(edges, point_groups, max_connections);

    // Step 4: Compute the size of each group.
    std::vector<int> group_sizes = std::vector<int>(group_count, 0);
    find_group_sizes(point_groups, group_sizes);

    // Step 5: Find the three largest groups.
    std::ranges::sort(group_sizes, std::greater<>{});
    int result = group_sizes[0] * group_sizes[1] * group_sizes[2];

    return Solution { result };
}
