#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    A simple breadth-first search, where in every step we press every
    button for every input value, and use the resulting list of output
    values as the input value of the next step. Since we're interested
    in the minimum number of steps, we can return the step count as
    soon as we encounter the target value. A memoization set serves
    to skip values that were encountered before.

    As a fairly minor optimization, we parse the target value as binary,
    using the left-most character as the LSB. Similarly, we parse each
    button into a bit mask, where a one means that the corresponding
    light is toggled. When we press a button, we simply perform a
    bitwise XOR between the light bits and the button bit mask.

    This could probably be optimized further; we do not take into account
    the fact that there is never any reason to press a button more than
    once. Still, the current solution is reasonably fast (<10ms).
*/

// Struct containing the target value and the list of button masks.

struct Machine {
    uint64_t target;
    std::vector<uint64_t> buttons;
};

// Parse the target value; the LSB is the first character.

uint64_t parse_target_value(std::string_view sv) {
    uint64_t value = 0;

    for (size_t shift = 0; shift < sv.length(); ++shift) {
        if (sv[shift] == '#') {
            value += 1 << shift;
        }
    }

    return value;
}

// Parse the buttons of a single machine, creating a bit mask for each.

std::vector<uint64_t> parse_buttons(std::string_view sv) {
    size_t group_start_pos = sv.find('(');
    std::vector<uint64_t> output {};

    while (group_start_pos != sv.npos) {
        size_t group_end_pos = sv.find(')', group_start_pos + 1);
        size_t group_length = group_end_pos - group_start_pos - 1;
        std::string_view group_sv = sv.substr(group_start_pos + 1, group_length);
        std::vector<uint64_t> shifts = parse_values<uint64_t>(group_sv);
        uint64_t button_mask = 0;

        for (uint64_t shift : shifts) {
            button_mask |= 1 << shift;
        }

        output.push_back(button_mask);
        group_start_pos = sv.find('(', group_start_pos + 1);
    }

    return output;
}

// Parse the target value and buttons of a single machine (line).

Machine parse_machine(const std::string& line) {
    std::string_view line_sv = std::string_view(line);

    size_t target_start_pos = line_sv.find('[');
    size_t target_end_pos = line_sv.find(']', target_start_pos + 1);
    size_t target_length = target_end_pos - target_start_pos - 1;

    std::string_view target_sv = line_sv.substr(target_start_pos + 1, target_length);
    uint64_t target_value = parse_target_value(target_sv);

    size_t buttons_start_pos = line_sv.find('(', target_end_pos + 1);
    size_t buttons_end_pos = line_sv.rfind(')');
    size_t buttons_length = buttons_end_pos - buttons_start_pos + 1;

    std::string_view buttons_sv = line_sv.substr(buttons_start_pos, buttons_length);
    std::vector<uint64_t> buttons = parse_buttons(buttons_sv);

    return Machine { target_value, buttons };
}

// Solve the current machine using a breadth-first approach with memoization.
// Since we use a breadth-first search, we can return the current step count
// as soon as we encounter the target value. Each value in the input queue
// of the current step is transformed used each of the available buttons,
// and the resulting output queue becomes the new input queue.

int solve_machine(const Machine& machine) {
    std::vector<uint64_t> current_queue { 0 };
    std::vector<uint64_t> next_queue {};
    std::set<uint64_t> memo {};
    int step_count = 0;

    while (true) {
        for (uint64_t value : current_queue) {
            if (value == machine.target) {
                return step_count;
            } else if (memo.contains(value)) {
                continue;
            }

            memo.insert(value);

            for (uint64_t button_mask : machine.buttons) {
                uint64_t next_value = value ^ button_mask;
                next_queue.push_back(next_value);
            }
        }

        current_queue.assign_range(next_queue);
        step_count++;
    }

    return 0;
}

// Main solver function; solve each machine (line), and sum up the results.

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int total = 0;

    for (const std::string& line : lines) {
        Machine machine = parse_machine(line);
        total += solve_machine(machine);
    }

    return Solution { total };
}
