#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "../../solution.hpp"

/*
    Very similar to the first part, but instead of a boolean indicating if
    the current index contains a beam, we use a long indicating in how many
    timelines this index contains a beam (we need to use longs instead of ints
    due to the magnitude of the answer). Whenever we encounter a splitter, we
    increase the timeline counts at i-1 and i+1 on the next row by the count
    at i on the current row. If there is no splitter, we add the count at i
    on the current row to that at i on the next one. Finally, we calculate
    the sum of all timelines on the final row.
*/

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    size_t row_length = lines.front().size();
    std::vector<long> timelines_a = std::vector<long>(row_length, 0);
    std::vector<long> timelines_b = std::vector<long>(row_length, 0);
    timelines_a[lines.front().find('S')] = 1;

    std::vector<long>* current_timelines = &timelines_a;
    std::vector<long>* next_timelines    = &timelines_b;

    for (size_t row_index = 1; row_index < lines.size(); ++row_index) {
        const std::string& line = lines[row_index];

        next_timelines->assign(next_timelines->size(), 0);

        for (size_t col_index = 0; col_index < row_length; ++col_index) {
            long current = current_timelines->at(col_index);

            if (current > 0) {
                if (line[col_index] == '^') {
                    next_timelines->at(col_index - 1) = next_timelines->at(col_index - 1) + current;
                    next_timelines->at(col_index + 1) = next_timelines->at(col_index + 1) + current;
                } else {
                    next_timelines->at(col_index) = next_timelines->at(col_index) + current;
                }
            }
        }

        std::swap(current_timelines, next_timelines);
    }

    long total = std::ranges::fold_left(*current_timelines, 0, std::plus {});

    return Solution { total };
}
