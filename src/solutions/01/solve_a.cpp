#include <algorithm>
#include <ranges>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We first parse the input, converting lines starting with 'R' to positive
    integers and lines starting with 'L' to negative ones. We then iterate over
    these integers, and keep track of the current dail position (starting at 50)
    and using modulo-100 to deal with cases where the dail moves left past zero
    or right past 100. After every move, we check if the dail is currently in
    position zero, and we count the total number of times this happens.

    I opted for a `for_each()` iteration using a lambda that captures both
    `current` and `zeros` by reference. Using `fold_left()` may have been better
    from an immutability perspective, but it would require passing both the dail
    position and the current number of zeros as a combined accumulator, which is
    needlessly complicated. The current solution is simple and efficient, and I
    like the pipe notation used to lazily parse the input lines.
*/

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int current = 50;
    int zeros = 0;

    auto count_zeros = [&](int value) {
        current = (current + value) % 100;

        if (current == 0) {
            zeros += 1;
        }
    };
    
    std::ranges::for_each(lines | std::views::transform(parse_line), count_zeros);

    return Solution { zeros };
}
