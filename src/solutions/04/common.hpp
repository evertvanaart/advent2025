#pragma once

#include <vector>

struct Cell {
    bool is_roll;
    int neighbor_count;

    Cell() {
        is_roll = false;
        neighbor_count = 0;
    }

    bool is_accessible() const {
        return is_roll && neighbor_count < 4;
    }
};

std::vector<int> get_neighbor_offsets(int col_count);

void initialize_rolls(const std::vector<std::string>& lines, std::vector<Cell>& cells, int row_count, int col_count);

void initialize_counts(std::vector<Cell>& cells, int row_count, int col_count);
