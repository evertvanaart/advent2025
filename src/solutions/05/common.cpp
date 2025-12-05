#include "common.hpp"

#include <charconv>

// Convert a string view to a long.

long string_view_to_long(const std::string_view& string_view) {
    long result{};

    std::from_chars(string_view.data(), string_view.data() + string_view.size(), result);

    return result;
}

// Parse the range lines, creating a start node and an end node for each line.
// Stop when we encounter the empty line, and return the index of the line
// after that, i.e. of the first ingredients line.

size_t create_nodes_from_ranges(const std::vector<std::string>& lines, std::vector<Node>& nodes) {
    size_t line_index = 0;

    for (; line_index < lines.size(); ++line_index) {
        const std::string& line = lines[line_index];

        if (line.empty()) {
            return line_index + 1;
        }

        size_t dash_pos = lines[line_index].find('-');
        std::string_view start_sv = std::string_view(line).substr(0, dash_pos);
        std::string_view end_sv   = std::string_view(line).substr(dash_pos + 1);
        long start_id = string_view_to_long(start_sv);
        long end_id   = string_view_to_long(end_sv);

        nodes.push_back(Node { Node::NodeType::RangeStart, start_id });
        nodes.push_back(Node { Node::NodeType::RangeEnd,   end_id   });
    }

    return line_index;
}

// Comparator used to sort nodes, first by ID and then by type.

bool compare_nodes(const Node& a, const Node& b) {
    if (a.id == b.id) {
        switch (a.node_type) {
            case Node::NodeType::RangeStart:
                return true;
            
            case Node::NodeType::Ingredient:
                return b.node_type != Node::NodeType::RangeStart;
            
            case Node::NodeType::RangeEnd:
                return false;
        }
    } else {
        return a.id < b.id;
    }
}
