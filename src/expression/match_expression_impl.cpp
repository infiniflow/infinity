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

MatchExpression::MatchExpression(const std::string &fields, const std::string &matching_text, const std::string &options_text)
    : BaseExpression(ExpressionType::kMatch, std::vector<std::shared_ptr<BaseExpression>>()), fields_(fields), matching_text_(matching_text),
      options_text_(options_text) {}

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
    return h;
}

bool MatchExpression::Eq(const BaseExpression &other_base) const {
    if (other_base.type() != ExpressionType::kMatch) {
        return false;
    }
    const auto &other = static_cast<const MatchExpression &>(other_base);
    bool eq = (fields_ == other.fields_ && matching_text_ == other.matching_text_ && options_text_ == other.options_text_);
    return eq;
}

} // namespace infinity
