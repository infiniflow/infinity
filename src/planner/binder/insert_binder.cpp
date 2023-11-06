//
// Created by JinHai on 2022/8/12.
//

module;

import stl;
import base_expression;
import parser;
import bind_context;
import column_expression;
import function;

import infinity_exception;
import third_party;
import function_set;
import bind_alias_proxy;

module insert_binder;

namespace infinity {

SharedPtr<BaseExpression> InsertBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    return result;
}

SharedPtr<BaseExpression> InsertBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Error<PlannerException>("KNN expression isn't supported in insert clause");
}

// SharedPtr<BaseExpression>
// InsertBinder::BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
//     PlannerError("HavingBinder::BuildColRefExpr");
// }

} // namespace infinity