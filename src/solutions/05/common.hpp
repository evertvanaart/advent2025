#pragma once

#include <string_view>
#include <vector>

struct Node {
    enum NodeType { RangeStart, RangeEnd, Ingredient };

    NodeType node_type;
    long id;
};

long string_view_to_long(const std::string_view& string_view);

size_t create_nodes_from_ranges(const std::vector<std::string>& lines, std::vector<Node>& nodes);

bool compare_nodes(const Node& a, const Node& b);
