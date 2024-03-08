#pragma once

#include "options.hpp"

namespace irs {

std::pair<std::string, std::string> parse_option(const std::string_view &option) {
    size_t eq_idx = option.find_first_of('=', 0);
    if (eq_idx == std::string::npos) {
        return std::make_pair(std::string(option), "");
    } else {
        std::string option_name(option.substr(0, eq_idx));
        std::string option_value(option.substr(eq_idx + 1));
        return std::make_pair(option_name, option_value);
    }
}

void parse_options(const std::string_view &options_str, char delimiter, std::vector<std::pair<std::string, std::string>> &options) {
    options.clear();
    if (options_str.empty())
        return;
    size_t begin_idx = 0;
    size_t len = options_str.length();
    while (begin_idx < len) {
        size_t sem_idx = options_str.find_first_of(delimiter, begin_idx);
        if (sem_idx == std::string::npos) {
            auto field = parse_option(options_str.substr(begin_idx));
            options.push_back(field);
            break;
        } else {
            auto field = parse_option(options_str.substr(begin_idx, sem_idx - begin_idx));
            options.push_back(field);
            begin_idx = sem_idx + 1;
        }
    }
    return;
}

} // namespace irs