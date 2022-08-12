//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/asserter.h"
#include "group_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
GroupBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);

    // if(root_expression) {  } why we need root expression flag?
    {
        std::string expr_name = expr.getName();
        if(bind_context_ptr->groups_by_expr_.contains(expr_name)) {
            ResponseError("Duplicated group by expression: " + expr_name);
        }

        // Add the group by expression into bind context
        bind_context_ptr->groups_by_expr_[expr_name] = result;
        bind_context_ptr->groups_.emplace_back(result);
    }

    return result;
}

std::shared_ptr<BaseExpression>
GroupBinder::BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<BaseExpression> column_expr = ExpressionBinder::BuildColRefExpr(expr, bind_context_ptr);

    ResponseError("GroupBinder::BuildColRefExpr");
    return column_expr;
}

}
