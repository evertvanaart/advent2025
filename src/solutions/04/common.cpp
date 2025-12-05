#include "common.hpp"

// Get the one-dimension index offsets for the eight surrounding cells. We pad
// the original grid with one row/column of empty cells on each side, so the
// neighbor offsets are based on the padded column count.

std::vector<int> get_neighbor_offsets(int col_count) {
    int padded_col_count = col_count + 2;

    std::vector<int> neighbor_offsets { 
        -padded_col_count - 1,
        -padded_col_count,
        -padded_col_count + 1,
        -1,
        1,
        padded_col_count - 1,
        padded_col_count,
        padded_col_count + 1
    };

    return neighbor_offsets;
}

// For each cell in the original grid (i.e. excluded the empty padding cells),
// check the input lines to determine whether the cell contains a roll.

void initialize_rolls(const std::vector<std::string>& lines, std::vector<Cell>& cells, int row_count, int col_count) {
    for (int row = 0; row < row_count; ++row) {
        int base_index = (row + 1) * (col_count + 2);
        const std::string& line = lines[row];

        for (int col = 0; col < col_count; ++col) {
            int index = base_index + col + 1;
            char cell_char = line[col];

            cells[index].is_roll = (cell_char == '@');
        }
    }
}

// For each cell in the original grid (i.e. excluded the empty padding cells)
// containing a roll, increase the neighbor count of all eight neighboring cells.
// Note, the neighbor count is increased even for neighbors that do not contain a
// roll, we check if the cell contains a roll in the `is_accessible()` function.

void initialize_counts(std::vector<Cell>& cells, int row_count, int col_count) {
    std::vector<int> neighbor_offsets  = get_neighbor_offsets(col_count);

    for (int row = 0; row < row_count; ++row) {
        int base_index = (row + 1) * (col_count + 2);

        for (int col = 0; col < col_count; ++col) {
            int index = base_index + col + 1;

            if (cells[index].is_roll) {
                for (int offset : neighbor_offsets) {            
                    int neighbor_index = index + offset;
                    cells[neighbor_index].neighbor_count++;
                }
            }
        }
    }
}
