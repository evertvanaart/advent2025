#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "../../solution.hpp"

/*
    Kind of disappointing for a final day. I spent a lot of time thinking
    of smart ways to do this - smarter than exhaustively trying all possible
    placements, at least - but I came up short. After a while, I decided to
    just start implementing, and handle parsing and two obvious edge cases:

    1. If the number of non-overlapping 3x3 cells in a region is equal to or
       larger than the number of presents, the presents will obvisouly fit,
       since each present will have its own 3x3 cell.

    2. On the other hand, if the total size of the presents (i.e. the total
       number of # fields) is larger than the size of the region, the presents
       will obviously never fit, no matter how we place them.
    
    After implementing both edge cases, I decided to see how many non-trivial
    regions I had left, only to find that this number was zero; all regions fit
    either edge case, and the answer is the number of regions that fit case #1.

    I dislike solutions that only work for certain types of input; as a general
    rule, I want solutions to be input agnostic. This definitely isn't the case
    here, since even the sample input doesn't adhere to this pattern (in this
    sense, this is a rare question where the sample input is harder than the
    real input). Still, implementing a proper solution (let alone a performant
    one) would be hell, so I'm happy to leave it here.
*/

// Parse a string view to size_t

int string_view_to_size_t(const std::string_view& string_view) {
    size_t result{};

    std::from_chars(string_view.data(), string_view.data() + string_view.size(), result);

    return result;
}

// Struct describing a present (shape), including the number of # cells (count)

struct Shape {
    bool fields[3][3];
    int count { 0 };

    static Shape parse(const std::vector<std::string>& lines, size_t offset) {
        Shape shape {};
        
        for (size_t row = 0; row < 3; ++row) {
            for (size_t col = 0; col < 3; ++col) {
                char field = lines[row + offset][col];
                shape.fields[row][col] = (field == '#');
                shape.count += (field == '#') ? 1 : 0;
            }
        }

        return shape;
    }
};

// Struct describing a region, consisting of size and shape counts.

struct Region {
    size_t width;
    size_t height;
    std::vector<size_t> counts;

    static Region parse(const std::string& line) {
        size_t times_index = line.find('x');
        size_t colon_index = line.find(':');

        auto width_string = std::string_view(line).substr(0, times_index);
        auto height_string = std::string_view(line).substr(times_index + 1, colon_index - times_index);
        size_t width = string_view_to_size_t(width_string);
        size_t height = string_view_to_size_t(height_string);

        auto counts = std::ranges::to<std::vector>(line.substr(colon_index + 2)
            | std::views::split(' ')
            | std::views::transform([](auto&& str) -> std::string_view { return std::string_view(str); })
            | std::views::transform([](const std::string_view& sv) -> size_t { return string_view_to_size_t(sv); })
        );

        return Region {
            width,
            height,
            counts
        };
    }
};

// Parse the first part of the input, containing the shapes (presents).

std::vector<Shape> parse_shapes(const std::vector<std::string>& lines, size_t& index) {
    std::vector<Shape> shapes {};

    while (index < lines.size()) {
        const std::string& line = lines[index];

        // Only region lines contain an 'x', so if the current line contains an
        // 'x', we've parsed all shapes and reached the block of region lines.
        if (line.contains('x')) {
            break;
        }

        if (line.ends_with(':')) {
            Shape shape = Shape::parse(lines, index + 1);
            shapes.push_back(shape);
            index += 3;
        }

        index += 1;
    }

    return shapes;
}

// Parse the second part of the input, containing the regions.

std::vector<Region> parse_regions(const std::vector<std::string>& lines, size_t& index) {
    std::vector<Region> regions {};

    for (; index < lines.size(); ++index) {
        Region region = Region::parse(lines[index]);
        regions.push_back(region);
    }

    return regions;
}

// Return true if the total number of required shapes (presents) of a region is larger
// the number of 3x3 cells in this region. Used to eliminate trivial edge case #1.

bool cannot_fit_in_cells(const Region& region) {
    size_t nr_cells = (region.width / 3) * (region.height / 3);
    size_t total_shapes = std::ranges::fold_left(region.counts, 0, std::plus{});
    return total_shapes > nr_cells;
}

// Return true if the total number of fields in all required shapes of a region is
// less than or equal to the region size. Used to eliminate trivial edge case #2.

bool can_fit_total_count(const Region& region, const std::vector<Shape>& shapes) {
    size_t region_size = region.height * region.width;
    size_t total_count = 0;

    for (size_t index = 0; index < shapes.size(); ++index) {
        total_count += shapes[index].count * region.counts[index];
    }

    return total_count <= region_size;
}

// Main solver function.

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    size_t line_index = 0;

    std::vector<Shape> shapes = parse_shapes(lines, line_index);
    std::vector<Region> regions = parse_regions(lines, line_index);
    size_t initial_region_count = regions.size();
    int valid_count = 0;

    // Remove all regions for which all shapes trivially fit in non-overlapping 3x3 cells.
    std::vector<Region> non_fitting_regions = std::ranges::to<std::vector>(regions 
        | std::views::filter(cannot_fit_in_cells));

    valid_count += (initial_region_count - non_fitting_regions.size());

    // Remove all regions for which the total number of required cells is larger than the area.
    std::vector<Region> complicated_regions = std::ranges::to<std::vector>(non_fitting_regions 
        | std::views::filter([&shapes](const Region& region) { return can_fit_total_count(region, shapes); }));
    
    if (input_name == "input") {
        // For the real input, all regions are covered by either edge case.
        assert(complicated_regions.size() == 0);
    }
    
    return Solution { valid_count };
}
