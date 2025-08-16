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

export module infinity_core:expression_transformer;


import :base_expression;
import :conjunction_expression;

namespace infinity {

enum class VisitControlType { kInvalid, kVisit, kNotVisit };

// Transform expr_a AND expr_b AND expr_c into expressions array: [expr_a, expr_b, expr_c].
export std::vector<std::shared_ptr<BaseExpression>> SplitExpressionByDelimiter(const std::shared_ptr<BaseExpression> &expression, ConjunctionType conjunction_type);

export std::shared_ptr<BaseExpression> ComposeExpressionWithDelimiter(const std::vector<std::shared_ptr<BaseExpression>> &expressions,
                                                                ConjunctionType conjunction_type);

// Traverse the expression and it's child
void VisitExpression(const std::shared_ptr<BaseExpression> &expression, const std::function<VisitControlType(std::shared_ptr<BaseExpression> &child)> &visitor);

export void VisitExpression(const std::shared_ptr<BaseExpression> &expression, const std::function<void(std::shared_ptr<BaseExpression> &child)> &visitor);

} // namespace infinity
