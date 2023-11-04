//
// Created by JinHai on 2022/8/12.
//

module;

import stl;
import base_expression;
import parser;
import bind_context;
import expression_binder;

import infinity_exception;

module aggregate_binder;

namespace infinity {

SharedPtr<BaseExpression> AggregateBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    return result;
}

SharedPtr<BaseExpression> AggregateBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Error<PlannerException>("KNN expression isn't supported in in aggregate function");
}

} // namespace infinity
