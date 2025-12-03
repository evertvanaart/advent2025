#include <algorithm>
#include <cmath>
#include <ranges>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    While this sounds like a dynamic programming problem at first, there is
    never any reason to not be greedy, i.e. the highest value is constructed
    by always taking the highest available digit. There are two constraints:
    we cannot pick a digit that's to the left of the current digit (we always
    move to the right), and we cannot pick a digit that is too far towards the
    end of the line, since this might leave us with not enough remaining digits.
    More precisely, when picking the first digit, we cannot choose any of the
    last eleven digits, since this would allow us to create a number of at most
    eleven digits. In the second step, the length of this tail from which we
    cannot pick digits decreases to ten digits, then nine, etc.

    We can use a heap to quickly find the next highest digit. Elements in this
    heap are sorted first by digit value (descending), then by string position
    (descending), i.e. we always want the first available highest digit. During
    each step, we first pop all heap elements from the top that are to the left
    of our current position (since these can no longer by picked), and we use
    the first valid digit as the next digit in our output number. At the end
    of every step, the current position is updated to that of the new digit.

    To deal with the constraint of the tail of unavailable digits, we leave the
    last eleven digits of the line out of the initial heap, and put them in a
    separate vector. After every step, the leftmost of these tailing digits
    becomes available to be picked, and so we push it onto the heap.

    Assuming that the initial heap is created in O(N log N), and individual
    push and pop operations are O(log N), this gives us a total complexity
    of O(N log N) per line, where N is the length of the line.
*/

const size_t OUTPUT_DIGITS = 12;
const size_t TAIL_LENGTH = 11;

// Struct to track the character value and position of a digit.

struct Digit {
    char value;
    size_t pos;

    static bool compare(const Digit& a, const Digit& b) {
        return (a.value == b.value) ? (a.pos > b.pos) : (a.value < b.value);
    }
};

// Repeatedly pop digits from the heap until the position of the head is
// higher than the current position, then pop and return that head digit.

Digit get_max_digit(std::vector<Digit>& digits_heap, size_t min_pos) {
    while (digits_heap.front().pos < min_pos) {
        std::ranges::pop_heap(digits_heap, Digit::compare);
        digits_heap.pop_back();
    }

    Digit max_digit = digits_heap.front();
    std::ranges::pop_heap(digits_heap, Digit::compare);
    digits_heap.pop_back();
    return max_digit;
}

// First split the line into two vectors of digits, one main one and one
// containing a tail of the last eleven digits. Make the main vector into
// a heap, and then select the twelve output digits one by one, every time
// selecting the highest available digit and updating the current position.
// After every step, push the next tail digit onto the heap.

long solve_line(const std::string& line) {
    std::vector<Digit> digits_heap = std::vector<Digit>(line.length() - TAIL_LENGTH);
    std::vector<Digit> digits_tail = std::vector<Digit>(TAIL_LENGTH);

    for (size_t pos = 0; pos < line.length() - TAIL_LENGTH; ++pos) {
        digits_heap[pos] = Digit { line[pos], pos };
    }

    for (size_t pos = line.length() - TAIL_LENGTH; pos < line.length(); ++pos) {
        digits_tail[pos - (line.length() - TAIL_LENGTH)] = Digit { line[pos], pos };
    }

    std::ranges::make_heap(digits_heap, Digit::compare);

    long multiplier = std::pow((long) 10, OUTPUT_DIGITS - 1);
    long output_value = 0;
    size_t pos = 0;

    for (size_t step = 0; step < OUTPUT_DIGITS; ++step) {
        Digit max_digit = get_max_digit(digits_heap, pos);
        output_value += digit_to_int(max_digit.value) * multiplier;
        pos = max_digit.pos;
        multiplier /= 10;

        if (step < OUTPUT_DIGITS) {
            digits_heap.push_back(digits_tail[step]);
            std::ranges::push_heap(digits_heap, Digit::compare);
        }
    }

    return output_value;
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    long result = std::ranges::fold_left(lines | std::views::transform(solve_line), 0, std::plus{});
    return Solution { result };
}
