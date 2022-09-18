//
// Created by JinHai on 2022/8/5.
//

#include "case_expression.h"
#include "common/utility/infinity_assert.h"

#include <sstream>

namespace infinity {

CaseExpression::CaseExpression() : BaseExpression(ExpressionType::kCase, {}) {}

void
CaseExpression::AddCaseCheck(const std::shared_ptr<BaseExpression>& when_expr, const std::shared_ptr<BaseExpression>& then_expr) {
    case_check_.emplace_back(when_expr, then_expr);
}

void
CaseExpression::AddElseExpr(const std::shared_ptr<BaseExpression>& else_expr) {
    if(else_expr_ != nullptr) {
        PlannerError("else expression already been assigned before.");
    }
    else_expr_ = else_expr;
}

std::string
CaseExpression::ToString() const {
    std::stringstream ss;

    for(auto& check: case_check_) {
        ss << "When: " << check.when_expr_->ToString() << " Then: " << check.then_expr_->ToString() << std::endl;
    }
    ss << "Else: " << else_expr_->ToString() << std::endl;

    return ss.str();
}

LogicalType
CaseExpression::DataType() {
    // expression_common_type(then()->data_type(), otherwise()->data_type());
    return LogicalType(LogicalTypeId::kBigInt);
}

}
