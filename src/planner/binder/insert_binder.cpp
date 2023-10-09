//
// Created by JinHai on 2022/8/12.
//

#include "insert_binder.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<BaseExpression> InsertBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    return result;
}

SharedPtr<BaseExpression> InsertBinder::BuildKnnExpr(const KnnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    PlannerError("KNN expression isn't supported in insert clause")
}

// SharedPtr<BaseExpression>
// InsertBinder::BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
//     PlannerError("HavingBinder::BuildColRefExpr");
// }

} // namespace infinity