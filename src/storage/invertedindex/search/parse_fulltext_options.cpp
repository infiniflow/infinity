// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <vector>
module parse_fulltext_options;

import stl;
import infinity_exception;
import status;
import filter_value_type_classification;

namespace infinity {

// parse minimum_should_match
SingleMinimumShouldMatchOption ParseSingleMinimumShouldMatchOption(const std::string_view input_str) {
    SingleMinimumShouldMatchOption result{};
    const char *input_chars = input_str.data();
    const bool is_percentage = input_str.back() == '%';
    auto len = input_str.size();
    if (is_percentage) {
        --len;
    }
    u32 start_val_pos = 0;
    if (const auto separate_pos = input_str.find('<'); separate_pos != std::string_view::npos) {
        start_val_pos = separate_pos + 1;
        int parse_int_result = 0;
        const auto last_ptr = input_chars + separate_pos;
        const auto [ptr, ec] = std::from_chars(input_chars, last_ptr, parse_int_result);
        if (ec != std::errc{} || ptr != last_ptr) {
            RecoverableError(Status::SyntaxError(
                std::format("Invalid minimum_should_match parameter format: Failed to parse positive integer before '<' in option '{}'.",
                            input_str)));
        }
        if (parse_int_result <= 0) {
            RecoverableError(Status::SyntaxError(
                std::format("Invalid minimum_should_match parameter format: Expect positive integer before '<', but get: '{}'.", input_str)));
        }
        result.first = parse_int_result;
    }
    if (start_val_pos >= len) {
        RecoverableError(Status::SyntaxError(std::format("Invalid minimum_should_match parameter format: No value after '<' : '{}'.", input_str)));
    }
    {
        int parse_int_result = 0;
        const auto last_ptr = input_chars + len;
        const auto [ptr, ec] = std::from_chars(input_chars + start_val_pos, last_ptr, parse_int_result);
        if (ec != std::errc{} || ptr != last_ptr) {
            RecoverableError(Status::SyntaxError(
                std::format("Invalid minimum_should_match parameter format: Failed to parse integer value in option '{}'.", input_str)));
        }
        if (is_percentage) {
            if (parse_int_result > 100 || parse_int_result < -100) {
                RecoverableError(Status::SyntaxError(
                    std::format("Invalid minimum_should_match parameter format: Percentage value out of range in option '{}'.", input_str)));
            }
            result.second.emplace<MinimumShouldMatchPercentage>(static_cast<i8>(parse_int_result));
        } else {
            result.second.emplace<MinimumShouldMatchCount>(parse_int_result);
        }
    }
    return result;
}

MinimumShouldMatchOption ParseMinimumShouldMatchOption(const std::string_view input_str) {
    MinimumShouldMatchOption result;
    const auto len = input_str.size();
    for (u32 i = 0; i < len; ++i) {
        if (input_str[i] != ' ') {
            u32 j = i + 1;
            while (j < len && input_str[j] != ' ') {
                ++j;
            }
            result.push_back(ParseSingleMinimumShouldMatchOption(input_str.substr(i, j - i)));
            i = j;
        }
    }
    std::sort(result.begin(), result.end(), [](const auto &left, const auto &right) { return left.first < right.first; });
    for (u32 i = 1; i < result.size(); ++i) {
        if (result[i].first == result[i - 1].first) {
            RecoverableError(Status::SyntaxError(std::format("Invalid minimum_should_match parameter format: Duplicate range in '{}'.", input_str)));
        }
    }
    return result;
}

u32 GetMinimumShouldMatchParameter(const MinimumShouldMatchOption &option_vec, const u32 leaf_count) {
    if (option_vec.empty()) {
        return 0;
    }
    auto pos = std::lower_bound(option_vec.begin(), option_vec.end(), leaf_count, [](const auto &val, const u32 cnt) { return val.first < cnt; });
    if (pos == option_vec.begin()) {
        return leaf_count;
    }
    auto [_, match_option] = *(--pos);
    auto get_result_from_i64 = [leaf_count](const i64 x) -> u32 {
        if (x >= 0) {
            return std::min(x, static_cast<i64>(leaf_count));
        } else {
            return std::max(x + static_cast<i64>(leaf_count), static_cast<i64>(0));
        }
    };
    return std::visit(Overload{[&](const MinimumShouldMatchCount cnt) -> u32 { return get_result_from_i64(cnt.value_); },
                               [&](const MinimumShouldMatchPercentage perc) -> u32 {
                                   if (perc.value_ > 100 || perc.value_ < -100) {
                                       UnrecoverableError("Fulltext minimum_should_match percentage out of range!");
                                   }
                                   auto cnt = (static_cast<i64>(perc.value_) * leaf_count) / 100;
                                   if (perc.value_ < 0 && cnt >= 0) {
                                       // special case
                                       cnt = leaf_count;
                                   }
                                   return get_result_from_i64(cnt);
                               }},
                      match_option);
}

} // namespace infinity
