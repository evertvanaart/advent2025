#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    Not that different from the first part. For each index of each column, we
    iterate backward through the first N-1 lines, and construct the number in
    that column by parsing the digit (using a simple numeric conversion, i.e.
    `c - '0'`) and multiplying it by a multiplier. We increase this multiplier
    by a factor ten after every digit, but not after a space; this way, we can
    use the same parsing logic regardless of whether the number is top-aligned
    or bottom-aligned. Once we've parsed all numbers, we use the same folding
    function as in the first part to obtain the result.
*/

long parse_number(const std::vector<std::string_view>& number_svs, size_t column_index) {
    long multiplier = 1;
    long value = 0;

    for (int row_index = number_svs.size() - 1; row_index >= 0; --row_index) {
        char digit_or_space = number_svs[row_index][column_index];

        if (digit_or_space == ' ') {
            continue;
        } else {
            value += multiplier * (long) (digit_or_space - '0');
            multiplier *= 10;
        }
    }

    return value;
}

long solve_column(const std::vector<std::string>& lines, size_t column_start, size_t column_end) {
    size_t column_width = column_end - column_start;

    auto number_svs = std::ranges::to<std::vector>(lines |
        std::views::take(lines.size() - 1) |
        std::views::transform([&](const std::string& line) -> std::string_view { 
            return std::string_view(line).substr(column_start, column_width); 
        })
    );

    std::vector<long> numbers = std::vector<long>(column_width);

    for (size_t index = 0; index < column_width; ++index) {
        numbers[index] = parse_number(number_svs, index);
    }

    char operand_char = get_operand_char(lines.back(), column_start);

    auto operand = (operand_char == '*') ? 
        [](long a, long b) -> long { return a * b; } :
        [](long a, long b) -> long { return a + b; };

    return std::ranges::fold_left(numbers | std::views::drop(1), numbers.front(), operand);
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    auto get_length = std::views::transform([](const std::string& line) -> size_t { return line.length(); });
    size_t max_length = std::ranges::max(lines | get_length);
    size_t column_start = 0;
    long total = 0;

    for (size_t index = 0; index < max_length; ++index) {
        if (is_all_spaces(lines, index)) {
            total += solve_column(lines, column_start, index);
            column_start = index + 1;
        }
    }

    total += solve_column(lines, column_start, max_length);
    return Solution { total };
}
