#include <algorithm>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We create a grid vector consisting of cells, with each cell consisting of
    a boolean indicating whether the cell contains a roll, and an integer for
    the number of neighboring cells containing a roll. Populating these cells
    is straightforward: we first check the input strings for '@' characters to
    determine rolls, then for each roll cell check all eight surrounding fields
    and count the neighboring rolls. Finally, we count the number of cells that
    contain an accessible roll, i.e. a roll with less than four neighbors.

    The only optimization worth mentioning is that we pad the initial grid on
    each side with a row or column of empty cells; this allows us the check the
    neighbors of the grid cells without having to constantly check for indices
    that are outside of the grid, which simplifies the logic and allows us to
    use one-dimensional indices throughout.
*/

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int row_count = lines.size();
    int col_count = lines.front().length();

    std::vector<Cell> cells = std::vector<Cell>((row_count + 2) * (col_count + 2));
    initialize_rolls(lines, cells, row_count, col_count);
    initialize_counts(cells, row_count, col_count);

    int total = std::ranges::count_if(cells, [](const Cell& cell) { return cell.is_accessible(); });

    return Solution { total };
}
