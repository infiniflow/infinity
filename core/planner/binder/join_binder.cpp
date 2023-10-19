//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import base_expression;
import parser;
import bind_context;
import infinity_assert;
import infinity_exception;

module join_binder;

namespace infinity {

SharedPtr<BaseExpression> JoinBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result;
    switch (expr.type_) {
        case ParsedExprType::kSubquery: {
            Error<PlannerException>("Subquery isn't allowed in JOIN condition.", __FILE_NAME__, __LINE__);
        }
        default: {
            result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
        }
    }
    return result;
}

SharedPtr<BaseExpression> JoinBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    Error<PlannerException>("KNN expression isn't supported in join clause.", __FILE_NAME__, __LINE__);
}

} // namespace infinity
