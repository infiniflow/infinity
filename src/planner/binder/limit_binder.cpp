//
// Created by JinHai on 2022/8/13.
//

#include "limit_binder.h"
#include "function/function_set.h"

namespace infinity {

SharedPtr<BaseExpression>
LimitBinder::BuildExpression(const ParsedExpr& expr,
                             BindContext* bind_context_ptr,
                             i64 depth,
                             bool root) {
    switch(expr.type_) {
        case ParsedExprType::kParameter: {
            PlannerError("Parameter expression isn't allowed in limit expression.");
        }
        case ParsedExprType::kSubquery: {
            PlannerError("Subquery expression isn't allowed in limit expression.");
        }
        default:
            return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    }
}

SharedPtr<BaseExpression>
LimitBinder::BuildFuncExpr(const FunctionExpr& expr,
                           BindContext* bind_context_ptr,
                           i64 depth,
                           bool root) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ != FunctionType::kScalar) {
        PlannerError("Only scalar function is supported in limit clause.");
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression>
LimitBinder::BuildColExpr(const ColumnExpr& expr,
                          BindContext* bind_context_ptr,
                          i64 depth,
                          bool root) {
    if(expr.star_) {
        PlannerError("Star expression isn't allowed in limit clause.");
    }
    return ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
}

}
