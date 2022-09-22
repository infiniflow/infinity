//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/infinity_assert.h"
#include "group_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
GroupBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result;

    switch(expr.type) {
        case hsql::kExprColumnRef : {
            result = BuildColExpr(expr, bind_context_ptr);
            break;
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
        }
    }

//    std::shared_ptr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);

    // if(root_expression) {  } why we need root expression flag?
    {
        std::string expr_name = expr.getName();
        if(bind_context_ptr->groups_by_expr_.contains(expr_name)) {
            PlannerError("Duplicated group by expression: " + expr_name);
        }

        // Add the group by expression into bind context
        bind_context_ptr->groups_by_expr_[expr_name] = result;
        bind_context_ptr->groups_.emplace_back(result);
    }

    return result;
}

std::shared_ptr<BaseExpression>
GroupBinder::BuildColRef (const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    // Bind order:
    // - try to find the column in current bind context bindings.
    // - try to find the column in current select list.
    // - try to find the column in outer bind context bindings.
    // ExpressionBinder::BuildColExpr will bind current context bindings and outer bind context.
    // So can't call ExpressionBinder::BuildColExpr directly.
    // TODO: need to split ExpressionBinder::BuildColExpr into 3 parts
    // - construct ColumnIdentifier
    // - build col expr in current bind context
    // - build col expr in parent bind context
    // All above 3 parts can be composed at will.

    // std::shared_ptr<BaseExpression> column_expr = ExpressionBinder::BuildColExpr(expr, bind_context_ptr);

    PlannerError("GroupBinder::BuildColRefExpr");
//    return column_expr;
}

}
