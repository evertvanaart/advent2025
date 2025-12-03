#include <algorithm>
#include <cstddef>
#include <functional>
#include <ranges>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We first create two vectors, both with length equal to the length of the
    current line. One vector contains the maximum digit encountered thus far
    when iterating through the string forward (from left to right), and one
    contains the maximum digit when iterating backward (right to left). For
    example, for "13524", the forward vector will be [1,3,5,5,5], and the
    backward vector will be [5,5,5,4,4].

    Once we've got both vectors, we iterate through both at the same time.
    For each position i, we combine the digits from the forward vector and
    the backward vector (using i + 1 as index, since we cannot use the same
    input digit twice) into a single two-digit number, and check whether its
    numeric value is the highest so far. In practice, for each position i we
    generate the highest possible value if the first digit is at or before i,
    and the second digit is at or after i + 1.

    We technically do not need the forward vector, since we could just track
    the maximum when doing the final iteration, but somehow removing it made
    the solution slightly slower.
 */

int solve_line(const std::string& line) {
    std::vector<char> max_digits_fw(line.length(), '0');
    std::vector<char> max_digits_bw(line.length(), '0');

    char max_fw = '0';
    char max_bw = '0';

    // Populate the forward and backward vectors.
    for (size_t fw_i = 0, bw_i = line.length() - 1; fw_i < line.length(); ++fw_i, --bw_i) {
        max_fw = std::max(max_fw, line[fw_i]);
        max_bw = std::max(max_bw, line[bw_i]);
        max_digits_fw[fw_i] = max_fw;
        max_digits_bw[bw_i] = max_bw;
    }

    int max_value = 0;

    // Combine the two arrays and track the maximum value.
    for (size_t i = 0; i < line.length() - 1; ++i) {
        int digit_fw = digit_to_int(max_digits_fw[i]);
        int digit_bw = digit_to_int(max_digits_bw[i + 1]);
        int combined = 10 * digit_fw + digit_bw;

        max_value = std::max(max_value, combined);
    }

    return max_value;
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int result = std::ranges::fold_left(lines | std::views::transform(solve_line), 0, std::plus{});
    return Solution { result };
}
