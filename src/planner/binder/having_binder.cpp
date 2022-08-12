//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/asserter.h"
#include "having_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
HavingBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {

    std::string expr_name = expr.getName();
    if(!this->in_aggregate_func_) {
        if (bind_context_ptr->groups_by_expr_.contains(expr_name)) {
            return bind_context_ptr->groups_by_expr_[expr_name];
        }
    }

    if(bind_context_ptr->aggregates_by_expr_.contains(expr_name)) {
        if(!this->in_aggregate_func_) {
            // not in an aggregate function
            return bind_context_ptr->aggregates_by_expr_[expr_name];
        } else {
            // in an aggregate function, which means aggregate function nested, which is error.
            ResponseError("HavingBinder::BuildExpression: Aggregate function is nested in another aggregate function.");
        }
    }

    return ExpressionBinder::BuildExpression(expr, bind_context_ptr);
}

std::shared_ptr<BaseExpression>
HavingBinder::BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<BaseExpression> column_expr = ExpressionBinder::BuildColRefExpr(expr, bind_context_ptr);

    ResponseError("HavingBinder::BuildColRefExpr");
    return column_expr;
}

}