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

#include <re2/re2.h>
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

String Trim(const String &str) {
    auto start = str.begin();
    auto end = str.end();

    while (start != end && std::isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }

    while (end != start && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    return String(start, end);
}

void Split(const std::string_view &input, const RE2 &pattern, Vector<String> &result, bool keep_delim = false) {
    re2::StringPiece leftover(input.data());
    re2::StringPiece last_end = leftover;
    re2::StringPiece extracted_delim_token;

    while (RE2::FindAndConsume(&leftover, pattern, &extracted_delim_token)) {
        std::string_view token(last_end.data(), extracted_delim_token.data() - last_end.data());
        if (!token.empty()) {
            result.push_back(Trim(String(token.data(), token.size())));
        }
        if (keep_delim)
            result.push_back(Trim(String(extracted_delim_token.data(), extracted_delim_token.size())));
        last_end = leftover;
    }

    if (!leftover.empty()) {
        result.push_back(Trim(String(leftover.data(), leftover.size())));
    }
}

void ParseRankFeatureOption(std::string_view input_str, RankFeatureOption &feature_option) {
    Vector<String> feature_strs;
    RE2 split_pattern{"(\\^)"};
    Split(input_str, split_pattern, feature_strs);

    if (feature_strs.size() == 2) {
        feature_option.field_ = feature_strs[0];
        feature_option.feature_ = feature_strs[1];
        feature_option.boost_ = 1.0f;
    } else if (feature_strs.size() == 3) {
        feature_option.field_ = feature_strs[0];
        feature_option.feature_ = feature_strs[1];
        const auto boost_str = feature_strs[2];
        try {
            feature_option.boost_ = std::stof(boost_str);
        } catch (const std::exception &e) {
            RecoverableError(
                Status::SyntaxError(std::format("Invalid rank_features parameter format: Failed to parse float value in option '{}'.", input_str)));
        }
    } else {
        RecoverableError(
            Status::SyntaxError(std::format("Invalid rank_features parameter format: Expect 3 parts separated by '^', but get: '{}'.", input_str)));
    }
}

RankFeaturesOption ParseRankFeaturesOption(std::string_view input_str) {
    RankFeaturesOption result;
    Vector<String> feature_strs;
    RE2 split_pattern{"(,)"};
    Split(input_str, split_pattern, feature_strs);
    for (auto &feature_str : feature_strs) {
        RankFeatureOption feature_option;
        ParseRankFeatureOption(feature_str, feature_option);
        result.push_back(feature_option);
    }
    return result;
}

} // namespace infinity
