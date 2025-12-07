#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "../../solution.hpp"

/*
    Since beams can only travel downward, we can iterate through the input
    line by line while tracking the positions of the beams on the current
    line using a vector of booleans (one per column).

    For each line, we iterate through all column indices. If the previous row
    has a beam at the current index i, we check if the line has a splitter '^'
    at i. If so, the beams at i-1 and i+1 become active; if not, the beam does
    not split, and the beam remains at i. If the previous row does not have a
    beam at i, we do nothing. Finally, we return the number of times we split
    the beam. There are some potential edge-cases that we do not handle (e.g.,
    what if there's two splitters next to each other?), but the question does
    not address these either, and the input doesn't contain such cases.

    As a minor optimization, we use two vectors for the current and next rows
    of booleans (indicating beam positions), and swap the two after every line;
    this way we do not need to allocate and de-allocate a new vector for every
    line, although we do still need to reset the current vector to all false.
*/

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    size_t row_length = lines.front().size();
    std::vector<bool> beams_a = std::vector<bool>(row_length, false);
    std::vector<bool> beams_b = std::vector<bool>(row_length, false);
    beams_a[lines.front().find('S')] = true;

    std::vector<bool>* current_beams = &beams_a;
    std::vector<bool>* next_beams    = &beams_b;

    int nr_splits = 0;

    for (size_t row_index = 1; row_index < lines.size(); ++row_index) {
        const std::string& line = lines[row_index];

        next_beams->assign(next_beams->size(), false);

        for (size_t col_index = 0; col_index < row_length; ++col_index) {
            if (current_beams->at(col_index)) {
                if (line[col_index] == '^') {
                    next_beams->at(col_index - 1) = true;
                    next_beams->at(col_index + 1) = true;
                    nr_splits++;
                } else {
                    next_beams->at(col_index) = true;
                }
            }
        }

        std::swap(current_beams, next_beams);
    }

    return Solution { nr_splits };
}
