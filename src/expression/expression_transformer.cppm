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

import :stl;
import :base_expression;
import :conjunction_expression;

namespace infinity {

enum class VisitControlType { kInvalid, kVisit, kNotVisit };

// Transform expr_a AND expr_b AND expr_c into expressions array: [expr_a, expr_b, expr_c].
export Vector<SharedPtr<BaseExpression>> SplitExpressionByDelimiter(const SharedPtr<BaseExpression> &expression, ConjunctionType conjunction_type);

export SharedPtr<BaseExpression> ComposeExpressionWithDelimiter(const Vector<SharedPtr<BaseExpression>> &expressions,
                                                                ConjunctionType conjunction_type);

// Traverse the expression and it's child
void VisitExpression(const SharedPtr<BaseExpression> &expression, const std::function<VisitControlType(SharedPtr<BaseExpression> &child)> &visitor);

export void VisitExpression(const SharedPtr<BaseExpression> &expression, const std::function<void(SharedPtr<BaseExpression> &child)> &visitor);

} // namespace infinity
