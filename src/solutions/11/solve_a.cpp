#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "../../solution.hpp"
#include "common.hpp"

/*
    We can use a recursive approach here. Let's say that nodes X and Y both
    have node Z as their output, and no other nodes output to Z. The number of
    paths leading to Z is therefore equal to the number of paths to X plus the
    number of paths to Y. This means that the function to find the number of
    paths to a current node should recurse to all of that node's input nodes,
    returning the sum of the results. We start this recursion at the last node
    ('out'), and stop when we reach the first node ('you'), or if the current
    node doesn't have any input nodes (in which case we return zero).

    To enable this algorithm, we first parse the lines, and essentially flip
    the mapping; instead of a map of nodes to their outputs, we create a map
    of nodes to their _input_ nodes, i.e. for node X we have a list of nodes
    that have X in their output list. We also convert the three-letter labels
    to zero-based indices, which are faster to compare and allow us to use
    a vector of vectors instead of a map of vectors, which should be faster.

    The final missing piece is memoization: after computing the number of paths
    to node X, we store the result in a memoization vector. At the start of the
    recursive function call, we can immediately return this memoized result if
    it is available, which cuts out a ton of redundant calculations. Without
    memoization, this algorithm can take minutes or even hours; with memo-
    ization, we finish in less than one millisecond.
 */

int recurse(
    const std::vector<std::vector<size_t>>& node_to_inputs,
    std::vector<int>& memo,
    size_t current_node,
    size_t target_node
) {
    if (current_node == target_node) {
        return 1;
    } else if (memo[current_node] != -1) {
        return memo[current_node];
    }

    int nr_paths = 0;

    for (size_t input_index : node_to_inputs[current_node]) {
        nr_paths += recurse(node_to_inputs, memo, input_index, target_node);
    }

    memo[current_node] = nr_paths;
    return nr_paths;
}

Solution solve(const std::vector<std::string>& lines, [[maybe_unused]] const std::string& input_name) {
    std::vector<std::vector<size_t>> node_to_inputs { lines.size() + 1 };
    std::vector<int> memo(node_to_inputs.size(), -1);
    std::map<std::string, size_t> label_to_index {};

    initialize(lines, node_to_inputs , label_to_index);
    size_t out_node_index = label_to_index["out"];
    size_t you_node_index = label_to_index["you"];

    int nr_paths = recurse(node_to_inputs, memo, out_node_index, you_node_index);

    return Solution { nr_paths };
}
