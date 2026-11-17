#pragma once

#include <vector>

#include <charconv>
#include <ranges>
#include <vector>
#include <string_view>

template <typename T> T parse_value(const std::ranges::subrange<const char *>&& value_sr) {
    T result{};

    std::string_view sv = std::string_view(value_sr);
    std::from_chars(sv.data(), sv.data() + sv.size(), result);

    return result;
}

template <typename T> std::vector<T> parse_values(std::string_view sv) {
    return std::ranges::to<std::vector>(sv
        | std::views::split(',')
        | std::views::transform(parse_value<T>));
}
