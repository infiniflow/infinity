//
// Created by JinHai on 2022/8/13.
//

module;

import stl;
import base_expression;
import parser;
import bind_context;
import function_set;
import function;
import infinity_assert;
import infinity_exception;

module limit_binder;

namespace infinity {

SharedPtr<BaseExpression> LimitBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    switch (expr.type_) {
        case ParsedExprType::kParameter: {
            Error<PlannerException>("Parameter expression isn't allowed in limit expression.", __FILE_NAME__, __LINE__);
        }
        case ParsedExprType::kSubquery: {
            Error<PlannerException>("Subquery expression isn't allowed in limit expression.", __FILE_NAME__, __LINE__);
        }
        default:
            return ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    }
}

SharedPtr<BaseExpression> LimitBinder::BuildFuncExpr(const FunctionExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(query_context_->storage()->catalog(), expr);
    if (function_set_ptr->type_ != FunctionType::kScalar) {
        Error<PlannerException>("Only scalar function is supported in limit clause.", __FILE_NAME__, __LINE__);
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression> LimitBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    if (expr.star_) {
        Error<PlannerException>("Star expression isn't allowed in limit clause.", __FILE_NAME__, __LINE__);
    }
    return ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);
}

SharedPtr<BaseExpression> LimitBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Error<PlannerException>("KNN expression isn't supported in limit clause", __FILE_NAME__, __LINE__);
}

} // namespace infinity
