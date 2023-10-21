//
// Created by JinHai on 2022/8/5.
//

module;

#include <sstream>

import stl;
import expression_type;
import parser;
import infinity_assert;
import infinity_exception;

module case_expression;

namespace infinity {

CaseExpression::CaseExpression() : BaseExpression(ExpressionType::kCase, {}) {}

void CaseExpression::AddCaseCheck(const SharedPtr<BaseExpression> &when_expr, const SharedPtr<BaseExpression> &then_expr) {
    case_check_.emplace_back(when_expr, then_expr);
}

void CaseExpression::AddElseExpr(const SharedPtr<BaseExpression> &else_expr) {
    if (else_expr_.get() != nullptr) {
        Error<PlannerException>("else expression already been assigned before.", __FILE_NAME__, __LINE__);
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
