#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace irs {

std::pair<std::string, std::string> parse_option(const std::string_view &option);

void parse_options(const std::string_view &options_str, char delimiter, std::vector<std::pair<std::string, std::string>> &options);

} // namespace irs