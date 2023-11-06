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

import stl;
import base_expression;
import parser;

export module case_expression;

namespace infinity {

export struct CaseCheck {
    CaseCheck(SharedPtr<BaseExpression> when_expr, SharedPtr<BaseExpression> then_expr)
        : when_expr_(Move(when_expr)), then_expr_(Move(then_expr)) {}

    SharedPtr<BaseExpression> when_expr_;
    SharedPtr<BaseExpression> then_expr_;
};

export class CaseExpression : public BaseExpression {
public:
    CaseExpression();

    void AddCaseCheck(const SharedPtr<BaseExpression> &when_expr, const SharedPtr<BaseExpression> &then_expr);

    void AddElseExpr(const SharedPtr<BaseExpression> &else_expr);

    DataType Type() const override;

    String ToString() const override;

    Vector<CaseCheck> &CaseExpr() { return case_check_; }

    SharedPtr<BaseExpression> &ElseExpr() { return else_expr_; }

    inline void SetReturnType(DataType type) { return_type_ = type; }

private:
    Vector<CaseCheck> case_check_;
    SharedPtr<BaseExpression> else_expr_;
    DataType return_type_{LogicalType::kInvalid};
};

} // namespace infinity
