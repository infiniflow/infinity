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

export module infinity_core:case_expression;


import :base_expression;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

export struct CaseCheck {
    CaseCheck(std::shared_ptr<BaseExpression> when_expr, std::shared_ptr<BaseExpression> then_expr)
        : when_expr_(std::move(when_expr)), then_expr_(std::move(then_expr)) {}

    std::shared_ptr<BaseExpression> when_expr_;
    std::shared_ptr<BaseExpression> then_expr_;
};

export class CaseExpression : public BaseExpression {
public:
    CaseExpression();

    void AddCaseCheck(const std::shared_ptr<BaseExpression> &when_expr, const std::shared_ptr<BaseExpression> &then_expr);

    void AddElseExpr(const std::shared_ptr<BaseExpression> &else_expr);

    DataType Type() const override;

    std::string ToString() const override;

    std::vector<CaseCheck> &CaseExpr() { return case_check_; }

    std::shared_ptr<BaseExpression> &ElseExpr() { return else_expr_; }

    inline void SetReturnType(DataType type) { return_type_ = type; }

private:
    std::vector<CaseCheck> case_check_;
    std::shared_ptr<BaseExpression> else_expr_;
    DataType return_type_{LogicalType::kInvalid};
};

} // namespace infinity
