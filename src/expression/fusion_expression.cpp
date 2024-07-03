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

#include <sstream>

module fusion_expression;

import stl;
import expression_type;

import scalar_function;
import search_options;
import infinity_exception;
import third_party;

namespace infinity {

FusionExpression::FusionExpression(const String &method, SharedPtr<SearchOptions> options)
    : BaseExpression(ExpressionType::kFusion, Vector<SharedPtr<BaseExpression>>()), method_(method), options_(std::move(options)) {}

String FusionExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    String expr_str = fmt::format("FUSION('{}', '{}')", method_, options_ ? options_->ToString() : "");
    return expr_str;
}

} // namespace infinity
