#include "common.hpp"

#include <charconv>

long string_view_to_long(const std::string_view& string_view) {
    long result{};

    std::from_chars(string_view.data(), string_view.data() + string_view.size(), result);

    return result;
}
