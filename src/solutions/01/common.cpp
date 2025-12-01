#include "common.hpp"

#include <charconv>
#include <string>
#include <string_view>

// Parse a `string_view` to an integer, used to parse the numeric part of the
// input line. We could also use `stoi(line.substr(1))`, but this requires an
// additional allocation for the sub-string. Using `string_view` avoids this
// allocation, and is therefore significantly faster.

int string_view_to_int(const std::string_view& string_view) {
    int result{};

    std::from_chars(string_view.data(), string_view.data() + string_view.size(), result);

    return result;
}

// Parse a line to an integer. The return value is equal to the parsed number
// if the line starts with 'R', or minus that number if line start with 'L'.

int parse_line(const std::string& line) {
    auto direction = line.front() == 'R' ? 1 : -1;
    auto value_string = std::string_view(line).substr(1);
    return direction * string_view_to_int(value_string);
}
