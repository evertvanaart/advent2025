#include <algorithm>
#include <charconv>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    This mostly just comes down to correctly parsing the input. We first search
    for the next index at which all lines contain a space, which gives us the
    start and end indices of a column. Next, we parse the numbers of the first
    N-1 lines in that column, and extract the operand character ('*' or '+')
    from the last line. Then, all we need to do is fold these numbers, using
    either addition or multiplication as the accumulator function.

    To obtain the column indices, we could have also looked at only the last
    line, since it seems that the index with all spaces is always one to the
    left of the index containing the operand character. The question doesn't
    specify this however, so I went with the slower but more correct approach.
    Similarly, we could have also just split the lines at groups of spaces,
    but this would not have worked if some columns contained fewer numbers
    than others, and the problem statement does not rule this out.

    Personal note: It's wild to me that the C++ standard library still does not
        have a function to trim whitespace from a string or string view, I feel
        like every modern language includes this in their standard library.
*/

// Remove whitespace from a string view before parsing it.

void trim_whitespace(std::string_view& sv) {
    sv.remove_prefix(std::min(sv.find_first_not_of(" "), sv.size()));
    sv.remove_suffix(std::min(sv.size() - sv.find_last_not_of(" ") - 1, sv.size()));
}

// Create a string view for a column and parse the number in this view.

long parse_number(const std::string& line, size_t column_start, size_t column_end) {
    long result{};

    std::string_view sv = std::string_view(line).substr(column_start, column_end - column_start);

    trim_whitespace(sv);

    std::from_chars(sv.data(), sv.data() + sv.size(), result);
    
    return result;
}

// Solve a column, defined by its start index (inclusive) and end index (exclusive).

long solve_column(const std::vector<std::string>& lines, size_t column_start, size_t column_end) {
    std::vector<long> numbers = std::ranges::to<std::vector>(lines | 
        std::views::take(lines.size() - 1) | 
        std::views::transform([&](const std::string& line) -> long 
        { return parse_number(line, column_start, column_end); }
    ));

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
