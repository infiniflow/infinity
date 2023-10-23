//
// Created by JinHai on 2022/8/5.
//

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
