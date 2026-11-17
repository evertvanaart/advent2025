#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <ranges>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    This one took a long time, in terms of both runtime and calendar time;
    I left it unfinished for almost a year, before finally returning to it
    in November of 2026. I initially tried to solve this using an approach
    similar to that of the A part, but since the possibility space is far
    too large for memoization, this clearly does not work.

    Instead, we transform the machine into a list of linear algebra equations.
    In each equation, the RHS is the target joltage, and the LHS contains the
    presses per button needed to reach that joltage. If 'a' is the number of
    times we press the first button, 'b' the second button, etc., we can
    express the first sample machine with the following equations:

        e + f     = 3
        b + f     = 5
        c + d + e = 4
        a + b + d = 7

    We use a recursive function to gradually solve these equations. If an
    equation has only a single button on the LHS, we can fix the number of
    presses of this button to the RHS. Similarly, if the RHS of an equation
    is zero, all buttons in the LHS must have zero presses.

    When there are no such trivial equations remaining, we instead pick a
    button, and iterate over all possible number of presses, creating a new
    set of equations for each press count. The maximum number of presses for
    a button is equal to the minimum RHS of all equations that use it, e.g.
    in the example above, 'f' has a maximum value of three. Picking a
    button from the smallest equation (i.e. lowest number of buttons)
    is generally fastest, since it eliminates more buttons quickly.

    This is still pretty slow; the above took over three hours to run.
    The big optimization is to simplify the equations after every step.
    For example, consider the following two equations:

        1)  a + b + c + d = 10
        2)  b + d = 3

    We can see that the buttons of equation #1 are a superset of those of
    equation #2. As such, we can replace #1 with a new equation: a + c = 7.
    The two equations still contain the same information, but the smaller
    equation #1 is much easier to solve. Performing this simplification
    step every time we lock in a button press count helped push the
    total runtime to below one second, even without multithreading.

    I do realize that I'm reinventing row reduction from first principles,
    and that a proper implementation of Gaussian elimination would probably
    be much faster, but given the amount of time I spent on this, I'm happy
    to have a working solution that runs in less than a second.
*/

struct Equation {
    std::vector<bool> buttons;
    int lhs_count;
    int rhs_value;

    // Create a new equation with the given number of buttons and RHS value.
    // The RHS value corresponds to the target joltage value.

    static Equation create(size_t nr_buttons, int rhs_value) {
        return Equation { std::vector<bool>(nr_buttons, false), 0, rhs_value };
    }

    // Initialize the equation using the list of input buttons. If a button
    // increments the current joltage value (based on my_index), it is enabled
    // in the vector of button booleans. Additionally, we track the number
    // of enabled buttons (lhs_count) for easy access.

    void initialize(const std::vector<std::vector<size_t>>& input_buttons, size_t my_index) {
        for (size_t button_index = 0; button_index < input_buttons.size(); ++button_index) {
            const std::vector<size_t>& current_button = input_buttons[button_index];
            
            if (std::ranges::contains(current_button, my_index)) {
                buttons[button_index] = true;
                lhs_count += 1;
            }
        }
    }

    // Get the button index of the first enabled button.

    size_t get_first_button_index() const {
        return std::ranges::find(buttons, true) - buttons.begin();
    }

    // An equation is invalid if the RHS value is negative, or the LHS is
    // empty (i.e. no more buttons) but the RHS value is not zero.

    bool is_invalid() const {
        return (rhs_value < 0) || ((lhs_count == 0) && (rhs_value != 0));
    }

    // An equation is finished if there are no buttons left in the LHS,
    // and the RHS value is equal to zero.

    bool is_finished() const {
        return (lhs_count == 0) && (rhs_value == 0);
    }

    // If the given button is used in this equation, remove it, and reduce
    // the RHS value by the given number of button presses.

    void apply_button(size_t button_index, int presses) {
        if (buttons[button_index]) {
            buttons[button_index] = false;
            rhs_value -= presses;
            lhs_count -= 1;
        }
    }
};

// Try to simplify two equations. We can simplify equations A and B if A uses
// a superset of the buttons of B. In this case, we can replace A with a new
// equation that uses the difference in buttons, with the RHS adjusted
// accordingly. Returns true if equation A was simplified in this way.

bool simplify_equation(std::vector<Equation>& equations, size_t button_count, size_t ia, size_t ib) {
    const Equation& a = equations[ia];
    const Equation& b = equations[ib];

    if (a.lhs_count <= b.lhs_count) {
        return false;
    }

    // If equation A can be simplified, the RHS of the new equation will be
    // equal to the RHS of A minus the RHS of B; already create this equation.
    Equation new_equation = Equation::create(button_count, a.rhs_value - b.rhs_value);

    for (size_t button_index = 0; button_index < button_count; ++button_index) {
        // B uses a button not used by A, so A is not a superset of B.
        if (b.buttons[button_index] && !a.buttons[button_index]) {
            return false;
        }
        
        // No difference in A and B for this button, continue with the next one.
        if (b.buttons[button_index]  == a.buttons[button_index]) {
            continue;
        }

        // Enable this button in the new equation.
        new_equation.buttons[button_index] = true;
        new_equation.lhs_count += 1;
    }

    equations[ia] = new_equation;
    return true;
}

// Simplify the list of equations by searching for equations A and B where the
// buttons of A are a superset of the buttons of B; if we find such a pair,
// replace A by a new equations that uses the difference in buttons.

void simplify_equations(std::vector<Equation>& equations, size_t button_count) {
    for (size_t i = 0; i < equations.size(); ++i) {
        for (size_t j = 0; j < equations.size(); ++j) {
            bool simplified = simplify_equation(equations, button_count, i, j);

            if (simplified) {
                continue;
            }
        }
    }
}

// Fix the number of presses of a button (indicated by its index), and update the list
// of equations accordingly. For all equations that use this button, the button is removed
// from the LHS, and the RHS is decreased with the number of presses. We then remove any
// equations that are 'finished' (i.e. empty LHS and zero RHS), and simplify the list of
// new equations in order to improve performance.

std::vector<Equation> apply(const std::vector<Equation>& equations, size_t button_count, size_t button_index, int presses) {
    std::vector<Equation> new_equations = equations;

    std::ranges::for_each(new_equations, 
        [&](Equation& eq) { return eq.apply_button(button_index, presses); });

    // Remove finished equations.
    const auto [first, last] = 
        std::ranges::remove_if(new_equations, [](Equation& eq) { return eq.is_finished();});
    new_equations.erase(first, last);

    // Simplify remaining equations.
    simplify_equations(new_equations, button_count);
    
    return new_equations;
}

// Return true is any of the equations is invalid.

bool is_any_invalid(const std::vector<Equation>& equations) {
    return std::ranges::any_of(equations, [](const Equation& eq) { return eq.is_invalid(); });
}

// Return the maximum number of presses for the given button, which is equal to
// the lowest RHS value of all equations that use this button.

int get_max_presses(const std::vector<Equation>& equations, size_t button_index) {
    return std::ranges::min(equations
        | std::views::filter([button_index](const Equation& eq) { return eq.buttons[button_index]; })
        | std::views::transform([](const Equation& eq) { return eq.rhs_value; }));
}

// Main recursion function.

int recurse(
    const std::vector<Equation>& equations,
    size_t button_count,
    int presses
) {
    if (!equations.empty() && is_any_invalid(equations)) {
        // One or more invalid equations.
        return INT_MAX;
    }

        if (equations.empty()) {
        // All equations have been solved correctly,
        // record the current number of presses.
        return presses;
    }

    for (const Equation& equation : equations) {
        // Option A: One of the remaining equations has only one button; we can fix
        // the number of presses of this button to the RHS of this equation.
        if (equation.lhs_count == 1) {
            int rhs_value = equation.rhs_value;
            size_t button_index = equation.get_first_button_index();
            std::vector<Equation> new_equations = apply(equations, button_count, button_index, rhs_value);
            return recurse(new_equations, button_count, presses + rhs_value);
        }

        // Option B: An equation has an RHS of zero; all buttons in this equation have
        // zero presses. We only fix the first button here, others will be fixed auto-
        // matically in subsequent recurse calls.
        if (equation.rhs_value == 0) {
            size_t button_index = equation.get_first_button_index();
            std::vector<Equation> new_equations = apply(equations, button_count, button_index, 0);
            return recurse(new_equations, button_count, presses);
        }
    }

    // No fixed count for any of the buttons in the remaining equations, so we have to iterate
    // over possible values. Pick the first button in the equation with the lowest number of
    // buttons, then determine the maximum number of times we can press this button, i.e.
    // the minimum RHS value of all equations that use this buttom.
    size_t target_button_index = 0;
    int least_buttons = INT_MAX;

    for (size_t ei = 0; ei < equations.size(); ++ei) {
        if (equations[ei].lhs_count < least_buttons) {
            least_buttons = equations[ei].lhs_count;
            target_button_index = equations[ei].get_first_button_index();
        }
    }

    int max_presses = get_max_presses(equations, target_button_index);
    int best_result = INT_MAX;

    // Recurse on all possible values of the selected button.
    for (int target_presses = max_presses; target_presses >= 0; --target_presses) {
        std::vector<Equation> new_equations = apply(equations, button_count, target_button_index, target_presses);
        int result = recurse(new_equations, button_count, presses + target_presses);
        best_result = std::min(best_result, result);
    }

    return best_result;
}

// Struct containing parsed machine information.

struct Machine {
    std::vector<Equation> equations;
    size_t button_count;
};

// Parse the buttons string to a vector of integer vectors.

std::vector<std::vector<size_t>> parse_buttons(std::string_view sv) {
    size_t group_start_pos = sv.find('(');
    std::vector<std::vector<size_t>> output {};

    while (group_start_pos != sv.npos) {
        size_t group_end_pos = sv.find(')', group_start_pos + 1);
        size_t group_length = group_end_pos - group_start_pos - 1;
        std::string_view group_sv = sv.substr(group_start_pos + 1, group_length);
        std::vector<size_t> indices = parse_values<size_t>(group_sv);

        output.push_back({ indices });
        group_start_pos = sv.find('(', group_start_pos + 1);
    }

    return output;
}

// Parse a line to a machine by constructing the set of equations.

Machine parse_machine(const std::string& line) {
    std::string_view line_sv = std::string_view(line);

    size_t target_start_pos = line_sv.find('[');
    size_t target_end_pos = line_sv.find(']', target_start_pos + 1);
    size_t buttons_start_pos = line_sv.find('(', target_end_pos + 1);
    size_t buttons_end_pos = line_sv.rfind(')');
    size_t buttons_length = buttons_end_pos - buttons_start_pos + 1;

    std::string_view buttons_sv = line_sv.substr(buttons_start_pos, buttons_length);
    std::vector<std::vector<size_t>> buttons = parse_buttons(buttons_sv);
    size_t button_count = buttons.size();

    size_t joltage_start_pos = line_sv.find('{', buttons_end_pos + 1);
    size_t joltage_end_pos = line_sv.find('}', joltage_start_pos + 1);
    size_t joltage_length = joltage_end_pos - joltage_start_pos - 1;

    std::string_view joltage_sv = line_sv.substr(joltage_start_pos + 1, joltage_length);
    std::vector<int> joltages = parse_values<int>(joltage_sv);
    std::vector<Equation> equations {};

    for (size_t joltage_index = 0; joltage_index < joltages.size(); ++joltage_index) {
        Equation new_equation = Equation::create(buttons.size(), joltages[joltage_index]);
        new_equation.initialize(buttons, joltage_index);
        equations.push_back(new_equation);
    }

    // Simplify initial list of solutions.
    simplify_equations(equations, button_count);

    return Machine { equations, button_count };
}

// Main solver function; loop over all machines and add up the results.

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    int total = 0;

    for (const std::string& line : lines) {
        Machine machine = parse_machine(line);

        int result = recurse(
            machine.equations,
            machine.button_count,
            0
        );

        total += result;
    }

    return Solution { total };
}
