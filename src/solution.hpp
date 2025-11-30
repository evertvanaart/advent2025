#pragma once

#include <string>
#include <variant>

typedef std::variant<int, double, std::string> Solution;

inline std::string stringify(Solution& solution) {
    if (int const* val = std::get_if<int>(&solution))
      return std::to_string(*val);
     
    if (double const* val = std::get_if<double>(&solution))
      return std::to_string(*val);

    if (std::string const* val = std::get_if<std::string>(&solution))
      return *val;
    
    return "<unsupported solution type>";
}
