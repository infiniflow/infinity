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

module infinity_core:match_expression.impl;

import :match_expression;

import :logger;
import :base_expression;
import :expression_type;

import std;
import third_party;

namespace infinity {

void ParseMultiIndexHints(const std::string &index_hints, std::vector<std::string> &index_names) {
    index_names.clear();
    if (index_hints.empty()) {
        return;
    }
    size_t begin_idx = 0;
    size_t len = index_hints.length();
    while (begin_idx < len) {
        size_t comma_idx = index_hints.find_first_of(',', begin_idx);
        if (comma_idx == std::string::npos) {
            auto index_name = index_hints.substr(begin_idx);
            index_names.emplace_back(index_name);
            break;
        } else {
            auto index_name = index_hints.substr(begin_idx, comma_idx - begin_idx);
            begin_idx = comma_idx + 1;
        }
    }
}

MatchExpression::MatchExpression(const std::string &fields, const std::string &matching_text, const std::string &options_text, const std::string &index_names)
    : BaseExpression(ExpressionType::kMatch, std::vector<std::shared_ptr<BaseExpression>>()), fields_(fields), matching_text_(matching_text),
      options_text_(options_text) {
    ParseMultiIndexHints(index_names, index_names_);
}

std::string MatchExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    std::string expr_str = fmt::format("MATCH TEXT ('{}', '{}', '{}'{})",
                                  fields_,
                                  matching_text_,
                                  options_text_,
                                  optional_filter_ ? fmt::format(", WHERE {}", optional_filter_->ToString()) : "");
    return expr_str;
}

u64 MatchExpression::Hash() const {
    u64 h = 0;
    h ^= std::hash<std::string>()(fields_);
    h ^= std::hash<std::string>()(matching_text_);
    h ^= std::hash<std::string>()(options_text_);
    for (size_t i = 0; i < index_names_.size(); i++) {
        h ^= std::hash<std::string>()(index_names_[i]);
    }
    return h;
}

bool MatchExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kMatch) {
        return false;
    }
    const auto &other = static_cast<const MatchExpression &>(other_base);
    bool eq = (fields_ == other.fields_ && matching_text_ == other.matching_text_ && options_text_ == other.options_text_ &&
               index_names_.size() == other.index_names_.size());
    if (eq) {
        for (size_t i = 0; i < index_names_.size(); i++) {
            if (index_names_[i] == other.index_names_[i]) {
                return false;
            }
        }
    }
    return eq;
}

} // namespace infinity
