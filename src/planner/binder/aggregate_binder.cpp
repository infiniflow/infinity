//
// Created by JinHai on 2022/8/12.
//

#include "aggregate_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
AggregateBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    return result;
}

std::shared_ptr<BaseExpression>
AggregateBinder::BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<BaseExpression> column_expr = ExpressionBinder::BuildColRefExpr(expr, bind_context_ptr);
    return column_expr;
}

}