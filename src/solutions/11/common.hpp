#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

size_t get_index(std::string label, std::map<std::string, size_t>& label_to_index);

void initialize(
    const std::vector<std::string>& lines,
    std::vector<std::vector<size_t>>& node_to_inputs,
    std::map<std::string, size_t>& label_to_index
);
