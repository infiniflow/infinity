//
// Created by JinHai on 2022/8/5.
//

#pragma once

#include <utility>

#include "base_expression.h"

namespace infinity {

struct CaseCheck {
    CaseCheck(std::shared_ptr<BaseExpression> when_expr, std::shared_ptr<BaseExpression> then_expr)
        : when_expr_(std::move(when_expr)), then_expr_(std::move(then_expr)) {}

    std::shared_ptr<BaseExpression> when_expr_;
    std::shared_ptr<BaseExpression> then_expr_;
};

class CaseExpression : public BaseExpression {
public:
    CaseExpression();
    void AddCaseCheck(const std::shared_ptr<BaseExpression>& when_expr, const std::shared_ptr<BaseExpression>& then_expr);
    void AddElseExpr(const std::shared_ptr<BaseExpression>& else_expr);

    LogicalType DataType() override;
    std::string ToString() const override;
private:
    std::vector<CaseCheck> case_check_;
    std::shared_ptr<BaseExpression> else_expr_;
};

}
