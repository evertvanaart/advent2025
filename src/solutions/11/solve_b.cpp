#include <cstddef>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We can reuse most of the approach of the first part by observing that we've
    got two possible routes consisting of three segments each:

    - Route A: From srv to dac, then to fft, then to out.
    - Route B: From srv to fft, then to dac, then to out.

    We can use our recursive function to find the number of paths for each of
    these six segments, and multiply them to get the count per route, e.g. the
    number of paths via route A is equal to the number of paths between srv and
    dac times the number of paths between dac and fft times the number of paths
    between fft and out (and similarly for route B), and each of those three
    counts can be computed using the same memoized recursive function.

    One crucial detail is these segments should not contain the other nodes of
    interest. For example, when calculating the number of paths in the segment
    between srv and dac, we should _not_ count paths that go through fft or
    out, since those will already be counted in other segments. In order to
    exclude paths containing those nodes, we pass two "forbidden" nodes to the
    recursive function, and return zero if we end up at a forbidden node; for
    example, in the segment from dac to out, both srv and fft are forbidden.

    Although we now need to call the recursive function six times instead of
    one, each of those six calls is 'lighter' than the one in the first part
    (since start and end nodes are closer together, and forbidden nodes give
    us more stopping conditions), so in practice this second part is only
    slightly slower than the first part.
  */

long recurse(
    const std::vector<std::vector<size_t>>& node_to_inputs,
    std::vector<int>& memo,
    size_t current_node,
    size_t target_node,
    size_t forbidden_node_a,
    size_t forbidden_node_b
) {
    if (current_node == target_node) {
        return 1;
    } else if (current_node == forbidden_node_a) {
        return 0;
    } else if (current_node == forbidden_node_b) {
        return 0;
    } else if (memo[current_node] != -1) {
        return memo[current_node];
    }

    long nr_paths = 0;

    for (size_t input_index : node_to_inputs[current_node]) {
        nr_paths += recurse(
            node_to_inputs,
            memo,
            input_index,
            target_node,
            forbidden_node_a,
            forbidden_node_b
        );
    }

    memo[current_node] = nr_paths;
    return nr_paths;
}

long find_path_count(
     const std::vector<std::vector<size_t>>& node_to_inputs,
    size_t current_node,
    size_t target_node,
    size_t forbidden_node_a,
    size_t forbidden_node_b
) {
    std::vector<int> memo(node_to_inputs.size(), -1);

    return recurse(
        node_to_inputs,
        memo,
        current_node,
        target_node,
        forbidden_node_a,
        forbidden_node_b
    );
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<std::vector<size_t>> node_to_inputs { lines.size() + 1 };
    std::map<std::string, size_t> label_to_index {};

    initialize(lines, node_to_inputs , label_to_index);
    size_t out = label_to_index["out"];
    size_t svr = label_to_index["svr"];
    size_t dac = label_to_index["dac"];
    size_t fft = label_to_index["fft"];

    long nr_paths_svr_to_dac = find_path_count(node_to_inputs, dac, svr, out, fft);
    long nr_paths_svr_to_fft = find_path_count(node_to_inputs, fft, svr, out, dac);
    long nr_paths_dac_to_fft = find_path_count(node_to_inputs, fft, dac, out, svr);
    long nr_paths_fft_to_dac = find_path_count(node_to_inputs, dac, fft, out, svr);
    long nr_paths_dac_to_out = find_path_count(node_to_inputs, out, dac, fft, svr);
    long nr_paths_fft_to_out = find_path_count(node_to_inputs, out, fft, dac, svr);

    long nr_paths_dac_fft = nr_paths_svr_to_dac * nr_paths_dac_to_fft * nr_paths_fft_to_out;
    long nr_paths_fft_dac = nr_paths_svr_to_fft * nr_paths_fft_to_dac * nr_paths_dac_to_out;

    return Solution { nr_paths_dac_fft + nr_paths_fft_dac };
}