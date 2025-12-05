#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We first create a vector of nodes, which each represent either the start
    of a range, the end of a range, or an ingredient. Each node consists of a
    node type and the ID value (using long because of the magnitude of values
    in the input). Once we've parsed the input and created all nodes, we sort
    this vector, first by ID (ascending), and then by type, where start nodes
    come before ingredient nodes, and then end nodes. This ordering of types
    allows us to correctly handle ingredients that sit on the end of a range.

    We then iterate through the sorted vector, keeping track of the depth, i.e.
    the number of ranges currently active, which increases at every start node
    and decreases at every end node. We then simply count of all ingredient
    nodes for which the current depth is larger than zero, i.e. ingredients
    that are inside at least one range and are therefore fresh.
 */

 // Parse the ingredient lines, adding ingredient nodes to the node vector.

void create_nodes_from_ingredients(const std::vector<std::string>& lines, std::vector<Node>& nodes, size_t line_index) {
    for (; line_index < lines.size(); ++line_index) {
        long ingredient_id = string_view_to_long(std::string_view(lines[line_index]));
        nodes.push_back(Node { Node::NodeType::Ingredient, ingredient_id });
    }
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<Node> nodes = std::vector<Node>();
    size_t line_index = create_nodes_from_ranges(lines, nodes);
    create_nodes_from_ingredients(lines, nodes, line_index);
    std::ranges::sort(nodes, compare_nodes);
    
    int count = 0;
    int depth = 0;

    for (const Node& node : nodes) {
        switch (node.node_type) {
            case Node::NodeType::RangeStart:
                depth++;
                break;

            case Node::NodeType::RangeEnd:
                depth--;
                break;

            case Node::NodeType::Ingredient:
                count += (depth > 0) ? 1 : 0;
                break;
        }
    }
    
    return Solution { count };
}
