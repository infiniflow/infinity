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
import stl;
import expression_type;
import parser;

import infinity_exception;

module case_expression;

namespace infinity {

CaseExpression::CaseExpression() : BaseExpression(ExpressionType::kCase, {}) {}

void CaseExpression::AddCaseCheck(const SharedPtr<BaseExpression> &when_expr, const SharedPtr<BaseExpression> &then_expr) {
    case_check_.emplace_back(when_expr, then_expr);
}

void CaseExpression::AddElseExpr(const SharedPtr<BaseExpression> &else_expr) {
    if (else_expr_.get() != nullptr) {
        Error<PlannerException>("else expression already been assigned before.");
    }
    else_expr_ = else_expr;
}

String CaseExpression::ToString() const {
    std::stringstream ss;

    for (auto &check : case_check_) {
        ss << "When: " << check.when_expr_->Name() << " Then: " << check.then_expr_->Name() << std::endl;
    }
    ss << "Else: " << else_expr_->Name() << std::endl;

    return ss.str();
}

DataType CaseExpression::Type() const { return return_type_; }

} // namespace infinity
