#pragma once

#include <string>
#include <vector>

bool is_all_spaces(const std::vector<std::string>& lines, size_t index);

char get_operand_char(const std::string& line, size_t column_start);
