#include "common.hpp"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

// Return the index for the given label using the label_to_index map. If the
// label is not yet in the map, add it with index value equal to the size of
// the map, to ensure that indices increment starting from zero.

size_t get_index(std::string label, std::map<std::string, size_t>& label_to_index) {
    auto iter = label_to_index.find(label);

    if (iter != label_to_index.end()) {
        return iter->second;
    }

    size_t new_index = label_to_index.size();
    label_to_index[label] = new_index;
    return new_index;
}

// Parse the input lines. Convert the three-letter labels to zero-based indices
// for faster access. For each node, create a list of the indices of the nodes
// that serve as its input. Since node indices are zero-based, we can return a
// vector of vectors instead of a map of vectors, which speeds up lookup.

void initialize(
    const std::vector<std::string>& lines,
    std::vector<std::vector<size_t>>& node_to_inputs,
    std::map<std::string, size_t>& label_to_index
) {
    for (const std::string& line : lines) {
        std::string source_label = line.substr(0, 3);
        size_t source_index = get_index(source_label, label_to_index);
        
        for (size_t start_pos = 5; start_pos < line.size(); start_pos += 4) {
            std::string output_label = line.substr(start_pos, 3);
            size_t output_index = get_index(output_label, label_to_index);
            node_to_inputs[output_index].push_back(source_index);
        }
    }
}
