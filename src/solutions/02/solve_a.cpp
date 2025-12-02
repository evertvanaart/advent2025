// clang incorrectly reports algorithm as unused
#include <algorithm> // IWYU pragma: keep
#include <cmath>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We parse the minimum and maximum values of each range, and additionally
    parse the value of their left half (LH), i.e. the numeric value of the
    left-most (N/2) digits, where N is the total number of digits (e.g.,
    for 123,456 the LH value is 123). We then have four possible cases:

    Case A: Minimum and maximum have equal length, and both lengths are odd.
        For example: 12,345 to 23,456

        In this case, the range does not contain any invalid IDs, since
        invalid IDs require an even number of digits.

    Case B: Minimum and maximum have equal length and equal LH value.
        For example: 123,000 to 123,456

        In this case, the range can contain at most one invalid ID, which
        is the LH value repeated twice, e.g. 123,123. We still need to check
        whether this potential invalid ID is actually inside the range.

    Case C: Minumum and maximum have equal length but different LH values.
        For example: 123,456 to 234,567.

        In this case, we iterate over the range of LH values (so from 123 to
        234 in this example), and create invalid IDs by repeating each LH value
        twice. We only need to check the first and last of these invalid IDs
        (so 123,123 and 234,234) against the range, since all other invalid
        IDs will always be inside the range.

    Case D: Minimum and maximum have different lengths.
        For example: 123 to 2,345, 12 to 34,567.

        We can handle this last case by splitting the range into two or more
        sub-ranges for which the minimum and maximum have the same length, and
        solving each sub-range independently. For example, 123 to 2,345 can be
        split into 123 to 999 and 1,000 to 2,345. If the difference in length
        is more than one, we add intermediate ranges, e.g. 12 to 34,567 can be
        split into 12 to 99, 100 to 999, 1,000 to 9,999, and 10,000 to 34,567.

    Note 1: This solution does assume that all input ranges are non-overlapping,
        which is not explicitly specified in the question. To make the solution
        work with overlapping ranges we'd have to add invalid IDs to a set, and
        then compute the sum of the values in the set.
    
    Note 2: Due to the size of the numbers in the input, we need to use longs.
*/

// Struct containing information about a limit (minimum or maximum):
// - The numeric value of the full limit string;
// - The numeric value of the left half (LH) of the limit string;
// - The multiplier that can be used to create invalid IDs (based on the
//   number of digits in the right half, e.g. for 12,345 the LH value will
//   be 12, the right half will be 345, and the multiplier will be 1000).

struct LimitInfo {
    long value;
    long lh_value;
    long multiplier;

    static LimitInfo parse(const std::string_view& sv) {
        size_t lh_digits = sv.length() / 2;
        long multiplier = std::pow(10, sv.length() - lh_digits);
        long lh_value = string_view_to_long(sv.substr(0, lh_digits));
        long value = string_view_to_long(sv);

        return LimitInfo { 
            value,
            lh_value,
            multiplier,
        };
    }
};

// Compute an invalid ID based on the LH value and the multiplier; e.g. an
// LH value of 123 with a multiplier of 1,000 results in invalid ID 123,123.

long compute_invalid_id(long lh_value, long multiplier) {
    return lh_value * multiplier + lh_value;
}

// Calculate the sum of all invalid IDs within a single range.

long solve_range(const std::string_view& min_sv, const std::string_view& max_sv) {
    if ((min_sv.length() == max_sv.length()) && (min_sv.length() % 2 != 0)) {
        // Case A: If the minimum and maximum limit contain the same number of
        // digits and this number is odd, this range will contain no invalid IDs.
        return 0;
    }

    auto min_limit = LimitInfo::parse(min_sv);
    auto max_limit = LimitInfo::parse(max_sv);

    if ((min_sv.length() == max_sv.length()) && (min_limit.lh_value == max_limit.lh_value)) {
        // Case B: If the minimum and maximum have the same LH value, there is
        // only one potential invalid ID, which is the LH value repeated twice.
        int invalid_id = compute_invalid_id(min_limit.lh_value, min_limit.multiplier);
        bool in_range = invalid_id >= min_limit.value && invalid_id <= max_limit.value;
        return in_range ? invalid_id : 0;
    } else if (min_sv.length() == max_sv.length()) {
        // Case C: If the two limits have the same (even) length, we can get all invalid
        // IDs by iterating from the LH of the minimum to the LH of the maximum; only the
        // first and last invalid IDs in this iteration need to be checked against the
        // range, all other invalid IDs are guaranteed to be in range.
        long invalid_id_sum = 0;
        long min_invalid_id = compute_invalid_id(min_limit.lh_value, min_limit.multiplier);
        long max_invalid_id = compute_invalid_id(max_limit.lh_value, max_limit.multiplier);
        invalid_id_sum += min_invalid_id >= min_limit.value ? min_invalid_id : 0;
        invalid_id_sum += max_invalid_id <= max_limit.value ? max_invalid_id : 0;
        
        for (long lh_value = min_limit.lh_value + 1; lh_value < max_limit.lh_value; lh_value++) {
            invalid_id_sum += compute_invalid_id(lh_value, min_limit.multiplier);
        }

        return invalid_id_sum;
    }

    // Case D: The two limits have different lengths. Split the range into two or more
    // sub-ranges, in such a way that the two limits of each sub-range have the same
    // length, and then solve for each sub-range, summing up the results.
    long invalid_id_sum = 0;

    for (auto length = min_sv.length(); length <= max_sv.length(); ++length) {
        long min_value = (length == min_sv.length()) ? min_limit.value : std::pow(10, length - 1);
        long max_value = (length == max_sv.length()) ? max_limit.value : std::pow(10, length) - 1;
        std::string min_str = std::to_string(min_value);
        std::string max_str = std::to_string(max_value);
        std::string_view min_sv = std::string_view(min_str);
        std::string_view max_sv = std::string_view(max_str);
        invalid_id_sum += solve_range(min_sv, max_sv);
    }

    return invalid_id_sum;
}

// Parse and solve a single range string, e.g. "123-234". First find the
// position of the dash, and then create one string view per limit value.

long solve_group(const std::ranges::subrange<const char *>&& group) {
    std::string_view group_sv = std::string_view(group);
    size_t dash_pos = group_sv.find('-');

    auto min_sv = group_sv.substr(0, dash_pos);
    auto max_sv = group_sv.substr(dash_pos + 1);

    return solve_range(min_sv, max_sv);
}

// Split the ranges by commas, compute the sum of invalid IDs in each
// range, and compute the sum of sums as the final answer.

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    const std::string& first_line = lines.front();

    auto results = std::string_view(first_line)
        | std::views::split(',')
        | std::views::transform(solve_group);

    long total = std::ranges::fold_left(results, 0, std::plus{});

    return Solution { total };
}
