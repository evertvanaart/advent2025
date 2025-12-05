#include <algorithm>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    The rare Advent of Code challenge where the second part is both easier and
    faster than the first part. We use the same approach as in the first part,
    but leaving out the ingredient nodes. After sorting the list of start and
    end nodes, we again iterate through the sorted vector, keeping track of the
    depth. Whenever this depth changes from zero to one (i.e. going from spoiled
    to fresh) we record the start ID, and whenever the depth goes from one to
    zero (fresh to spoiled) we add the total size of the combined range from
    the stored start ID to the current node's end ID.

    Like in the first part, this is a simple O(N log N) sorting step followed by
    an O(N) iteration, and in this second part N is only in the order of ~400.
 */

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<Node> nodes = std::vector<Node>();
    create_nodes_from_ranges(lines, nodes);
    std::ranges::sort(nodes, compare_nodes);

    long start = 0;
    long count = 0;
    int depth = 0;

    for (const Node& node : nodes) {
        if (node.node_type == Node::NodeType::RangeStart) {
            if (depth == 0) {
                start = node.id;
            }

            depth++;
        } else {
            if (depth == 1) {
                count += node.id - start + 1;
            }

            depth--;
        }

    }

    return Solution { count };
}
