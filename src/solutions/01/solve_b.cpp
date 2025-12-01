#include <algorithm>
#include <ranges>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We could calculate every position of the dail and check if it's zero, but
    this is clearly not efficient. Instead, we calculate the number of zeros
    passed during a single move (i.e. a single line) by looking only at the
    start and end positions. We no longer apply modulo-100 after every move,
    and instead see the dail as a continuous integer. For each dail position,
    we can compute the position's cycle count, which is the total number of
    completed rotations to the right. Positions 0 to 99 (inclusive) have cycle
    count 0, 100-199 have cycle count 1, etc; for negative dail positions, -1
    to -100 (inclusive) is cycle -1, -101 to -200 is -2, etc.

    The number of zeros we pass when turning the dail to the right, i.e. when
    the dail position increases, is equal simply to the cycle of the end position
    minus the cycle of the start position; for example, when turning the dail from
    50 to 250, the cycle difference is two, and we passed two zero positions. This
    also works when we start or end on a zero; for example, moving right from 0 to
    10 has a cycle difference of 0, which is correct since we should not count the
    initial zero position for the current move. On the other hand, if we move from
    90 to 100, the final zero position does count, and the cycle difference is one.

    For negative moves, i.e. turning the dail to the left, this is slightly more
    complicated. Turning the dail left from 10 to 0 should count as one zero (since
    we end on a zero), but the cycle difference is zero. Similarly, turning the dail
    from 100 to 90 should not count as a zero (since the starting zero doesn't count),
    but the cycle difference is one. As it turns out, we can fix both edge-cases by
    subtracting one from both the start and the end positions for negative moves,
    i.e. these two examples become 9 to -1 (cycle difference one) and 99 to 89
    (cycle difference zero), respectively.
*/

// Get the cycle count of the current position. For positive positions, this is
// simply the position divided by 100. For negative positions, we adjust it so
// that -1 to -100 are cycle -1, -101 to -200 are cycle -2, and so on.

int get_cycle(int position) {
    return position >= 0 ? (position / 100) : ((position + 1) / 100) - 1;
}

// Count the number of zeros passed (or ended on) during a positive move, i.e when
// turning the dail to the right. For positive moves, this is equal to the differ-
// ence between the cycle counts of the start and end positions.

int count_zeros_pos(int start, int end) {
    int start_cycle = get_cycle(start);
    int end_cycle = get_cycle(end);
    return end_cycle - start_cycle;
}

// Count the number of zeros passed (or ended on) during a negative move, i.e when
// turning the dail to the left. For negative moves, we have to subtract one from
// the start and end positions in order to avoid the edge cases mentioned above.

int count_zeros_neg(int start, int end) {
    int start_cycle = get_cycle(start - 1);
    int end_cycle = get_cycle(end - 1);
    return start_cycle - end_cycle;
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int current = 50;
    int zeros = 0;

    auto count_zeros = [&](int value) {
        int next = current + value;

        zeros += (value >= 0) ? 
            count_zeros_pos(current, next) :
            count_zeros_neg(current, next);
        
        current = next;
    };

    std::ranges::for_each(lines | std::views::transform(parse_line), count_zeros);

    return Solution { zeros };
}
