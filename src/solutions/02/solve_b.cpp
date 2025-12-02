// clang incorrectly reports algorithm as unused
#include <algorithm> // IWYU pragma: keep
#include <cmath>
#include <cstddef>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    If the minimum and maximum of a range have the same length, we now iterate
    over all prefix lengths that are a divisor of the min/max length (i.e. the
    min/max length modulo the prefix length is zero); this also means that we
    do not have to check any prefixes longer than half the min/max length.

    For each prefix length, we compute the prefix value of that length for both
    the minimum and maximum, and iterate over that range. For example, for the
    range 123,456 to 234,567 and prefix length 2, we iterate from 12 to 23. For
    each prefix value, we generate an invalid ID by repeating this prefix value
    N times, where N is the min/max length divided by the prefix length, e.g.
    in the example above we generate 121,212, 131,313, and so on. Like in the
    first part, we only need to check the first and last invalid IDs against
    the range, all other invalid IDs will always be inside the range.

    If the lengths of the minimum and maximum value are not the same, we first
    split the range into two or more sub-ranges, same as Case D in the A part.

    The same invalid ID can be generated multiple times for different prefix
    lengths. For example, if we take the range 200,000 to 300,000, the invalid
    ID 222,222 can be generated at prefix lengths 1, 2, and 3. As such, we need
    to deduplicate the invalid IDs using a set before calculating their sum.
    We create a new set for each range, so the solution still does not
    properly handle overlapping ranges.
*/

// Struct containing the numeric and string values of a limit.

struct Limit {
    std::string_view sv;
    long value;

    static Limit parse(const std::string_view& sv) {
        return Limit { sv, string_view_to_long(sv) };
    }
};

// Struct containing the minimum and maximum (both inclusive) of a range.

struct Range {
    Limit min;
    Limit max;

    bool contains(long value) const {
        return (value >= min.value) && (value <= max.value);
    }

    static Range parse(const std::string_view& min_sv, const std::string_view& max_sv) {
        return Range { Limit::parse(min_sv), Limit::parse(max_sv) };
    }
};

// Compute an invalid ID from a prefix value by repeating it N times. For example,
// if the prefix value is 12, the prefix length will be 2 (since we have two digits);
// if the repeat count is set to 3, the output invalid ID will be 121,212.

long compute_invalid_id(long prefix_value, size_t prefix_length, size_t repeat_count) {
    long multiplier = 1;
    long output = 0;

    for (size_t i = 0; i < repeat_count; ++i) {
        output += multiplier * prefix_value;
        multiplier *= std::pow(10, prefix_length);
    }

    return output;
}

// Calculate the sum of all invalid IDs in the input range for a specific prefix
// length. For example, for the range 123,456 to 234,567 and prefix length 3, we
// iterate from 123 to 234, and create an invalid ID for each prefix value, i.e.
// 123,123, 124,124, etc. We only need to check the first and last invalid IDs
// against the range, all other IDs are always inside the range.

void solve_for_prefix_length(
    const Range& range,
    size_t prefix_length,
    size_t repeat_count,
    std::set<long>& invalid_ids
) {
    std::string_view min_prefix_sv = range.min.sv.substr(0, prefix_length);
    std::string_view max_prefix_sv = range.max.sv.substr(0, prefix_length);
    long min_prefix_value = string_view_to_long(min_prefix_sv);
    long max_prefix_value = string_view_to_long(max_prefix_sv);

    for (long v = min_prefix_value; v <= max_prefix_value; ++v) {
        long invalid_id = compute_invalid_id(v, prefix_length, repeat_count);
        bool is_first_or_last = (v == min_prefix_value) || (v == max_prefix_value);

        if (!is_first_or_last || range.contains(invalid_id)) {
            invalid_ids.insert(invalid_id);
        }
    }
}

// Calculate all invalid IDs for all possible prefix lengths for a given range.
// Use a set to track previously discovered invalid IDs to avoid duplicates.

long solve_range(Range range) {
    if (range.min.sv.length() != range.max.sv.length()) {
        // Like Case D of the A part, if the minimum and maximum have different
        // numbers of digits, divide the range into sub-ranges with minimum and
        // maximum of equal length, and solve sub-ranges separately.
        long invalid_id_sum = 0;

        for (auto length = range.min.sv.length(); length <= range.max.sv.length(); ++length) {
            long min_value = (length == range.min.sv.length()) ? range.min.value : std::pow(10, length - 1);
            long max_value = (length == range.max.sv.length()) ? range.max.value : std::pow(10, length) - 1;

            std::string min_str = std::to_string(min_value);
            std::string max_str = std::to_string(max_value);
            
            std::string_view min_sv = std::string_view(min_str);
            std::string_view max_sv = std::string_view(max_str);

            Limit min_limit = Limit { min_sv, min_value };
            Limit max_limit = Limit { max_sv, max_value };

            invalid_id_sum += solve_range(Range{ min_limit, max_limit });
        }

        return invalid_id_sum;
    }

    size_t length = range.min.sv.length();
    std::set<long> invalid_ids { };

    for (size_t prefix_length = 1; prefix_length <= (length / 2); ++prefix_length) {
        if ((length % prefix_length) == 0) {
            size_t repeat_count = length / prefix_length;

            solve_for_prefix_length(
                range, 
                prefix_length,
                repeat_count,
                invalid_ids
            );
        }
    }

    return std::ranges::fold_left(invalid_ids, 0, std::plus {});
}

// Parse and solve a single range string, e.g. "123-234". First find the
// position of the dash, and then create one string view per limit value.

long solve_group(const std::ranges::subrange<const char *>&& group) {
    std::string_view group_sv = std::string_view(group);
    size_t dash_pos = group_sv.find('-');

    auto min_sv = group_sv.substr(0, dash_pos);
    auto max_sv = group_sv.substr(dash_pos + 1);
    auto range = Range::parse(min_sv, max_sv);

    return solve_range(range);
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
