#include <cstddef>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    First initialize the cells like in the first part, then populate a queue
    with indices of cells that are already accessible. For each cell in this
    queue, remove the roll. Next, for each neighbor of this cell, reduce the
    neighbor count by one; if this causes a neighboring roll to become access-
    ible, add its index to the queue. Keep repeating this until the queue is
    empty, and finally return the number of rolls removed.

    While this is probably not the most efficient approach, it's still better
    than doing a full grid scan after every removal step. I originally used a
    deque for the queue, but I found that a regular vector with an index to
    track processed elements is slightly faster. I also tried using a set,
    since a vector/deque-based queue will contain a substantial number of
    duplicates, but this was slower than either alternative.
*/

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int row_count = lines.size();
    int col_count = lines.front().length();

    std::vector<Cell> cells = std::vector<Cell>((row_count + 2) * (col_count + 2));
    initialize_rolls(lines, cells, row_count, col_count);
    initialize_counts(cells, row_count, col_count);

    std::vector<int> neighbor_offsets = get_neighbor_offsets(col_count);
    std::vector<size_t> remove_index_queue  = std::vector<size_t>();
    remove_index_queue.reserve(cells.size());
    size_t queue_index = 0;
    int rolls_removed = 0;

    // Populate the initial queue with accessible cell indices.
    for (size_t index = 0; index < cells.size(); ++index) {
        if (cells[index].is_accessible()) {
            remove_index_queue.push_back(index);
        }
    }

    while (queue_index < remove_index_queue.size()) {
        size_t index_to_remove = remove_index_queue[queue_index];
        Cell& cell_to_remove = cells[index_to_remove];
        queue_index++;

        // We may have already removed this roll in a previous iteration.
        if (!cell_to_remove.is_roll) {
            continue;
        }

        // Remove the roll and increase count.
        cell_to_remove.is_roll = false;
        rolls_removed++;

        for (int offset : neighbor_offsets) {
            // Reduce the neighboring roll count of the neighbor.
            size_t neighbor_index = index_to_remove + offset;
            Cell& neighbor_cell = cells[neighbor_index];
            neighbor_cell.neighbor_count--;

            if (neighbor_cell.is_accessible()) {
                // Neighboring roll is now accessible, add it to the queue.
                remove_index_queue.push_back(neighbor_index);
            }
        }
    }

    return Solution { rolls_removed };
}
