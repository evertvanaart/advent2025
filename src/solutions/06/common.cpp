#include "common.hpp"

#include <algorithm>
#include <string>
#include <vector>

bool is_space(const std::string& line, size_t index) {
    return index >= line.length() || line[index] == ' ';
}

bool is_all_spaces(const std::vector<std::string>& lines, size_t index) {
    return std::ranges::find_if_not(lines, 
            [&index](const std::string& line) -> bool 
            { return is_space(line, index); } 
        ) == lines.end();
}

char get_operand_char(const std::string& line, size_t column_start) {
    size_t operand_pos = line.find_first_not_of(" ", column_start);
    return line[operand_pos];
}
