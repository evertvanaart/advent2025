#include <cassert>
#include <chrono>
#include <fstream>
#include <print>
#include <string>
#include <vector>

#include "solution.hpp"

// Number of runs when profiling
const int PROFILE_RUNS = 20;

// Signature of the solve() function, to be implemented by the individual solution files.
//
// The Makefile is set up to include only the solution file for current day and part (e.g,
// 'solutions/01/solve_a.cpp' for day 1 and part A), so there will only ever be one target
// solve() implementation for the linker. The 'input_name' argument contains the base name
// of the input file (e.g. 'sample' or 'input'), which can for example be used if the
// sample input and the real input use different logic and/or constraints.

Solution solve(const std::vector<std::string>& lines, const std::string& input_name);

struct Arguments {
    std::string day;
    std::string input_name;
    bool do_profile;
};

std::vector<std::string> read_input_file(Arguments& arguments) {
    std::string filename = "data/" + arguments.day + "/" + arguments.input_name + ".txt";
    std::println("Reading input file '{}'...", filename);
    std::ifstream input_file(filename);
    assert(input_file);

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(input_file, line)) {
        lines.push_back(line);
    }

    return lines;
}

Arguments parse_arguments(int argc, char **argv) {
    assert(argc == 3 || argc == 4);
    std::string day_part = std::string(argv[1]);
    assert(day_part.length() == 3);
    std::string day = day_part.substr(0, 2);
    std::string input_name = std::string(argv[2]);
    bool do_profile = argc == 4 && std::string(argv[3]) == "profile";

    return Arguments { day, input_name, do_profile };
}

int main(int argc, char **argv) {
    auto arguments = parse_arguments(argc, argv);
    auto lines = read_input_file(arguments);

    if (arguments.do_profile) {
        std::println("Profiling solution...");
        double total_ms = 0.0;

        // ignore the time of the first run
        solve(lines, arguments.input_name);

        for (int i = 0; i != PROFILE_RUNS; i++) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            solve(lines, arguments.input_name);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
            auto duration_ms = (double) duration_ns.count() / 1000000.0;
            total_ms += duration_ms;
        }

        auto average_ms = total_ms / (double) PROFILE_RUNS;
        std::println("Average over {} runs: {:.3f} ms", PROFILE_RUNS, average_ms);
    } else {
        std::println("Running solution...");
        auto start_time = std::chrono::high_resolution_clock::now();

        auto solution = solve(lines, arguments.input_name);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        auto duration_ms = (double) duration_ns.count() / 1000.0;

        std::println("Solution: {}", stringify(solution));
        std::println("Completed in {:.3f} ms", duration_ms);
    }

    return 0;
}
