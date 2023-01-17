//
// Created by JinHai on 2022/8/13.
//

#include "limit_binder.h"
#include "function/function_set.h"

namespace infinity {

SharedPtr<BaseExpression>
LimitBinder::BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext> &bind_context_ptr) {
    switch(expr.type) {
        case hsql::kExprStar:
            PlannerError("Star expression isn't allowed in limit expression.");
        case hsql::kExprParameter:
            PlannerError("Parameter expression isn't allowed in limit expression.");
        case hsql::kExprColumnRef:
            PlannerError("Column expression isn't allowed in limit expression.");
        case hsql::kExprSelect:
            PlannerError("Subquery isn't allowed in limit expression.");
        case hsql::kExprHint:
            PlannerError("Hint expression isn't allowed in limit expression.");
        case hsql::kExprArray:
            PlannerError("Array expression isn't allowed in limit expression.");
        case hsql::kExprArrayIndex:
            PlannerError("Array Index expression isn't allowed in limit expression.");
        default:
            return ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    }
}

SharedPtr<BaseExpression>
LimitBinder::BuildFuncExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ != FunctionType::kScalar) {
        PlannerError("Only scalar function is supported in limit clause.");
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr);
}

}
