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

#include "search/filter.hpp"

import stl;
import parser;
import base_expression;
import expression_type;
import third_party;

module match_expression;

namespace infinity {

MatchExpression::MatchExpression(const String &fields, const String &matching_text, const String &options_text)
    : BaseExpression(ExpressionType::kMatch, Vector<SharedPtr<BaseExpression>>()), fields_(fields), matching_text_(matching_text),
      options_text_(options_text) {}

String MatchExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    String expr_str = Format("MATCH('{}', '{}', '{}')", fields_, matching_text_, options_text_);
    return expr_str;
}

} // namespace infinity
