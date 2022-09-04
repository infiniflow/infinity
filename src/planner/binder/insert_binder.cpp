//
// Created by JinHai on 2022/8/12.
//

#include "common/utility/infinity_assert.h"
#include "insert_binder.h"

namespace infinity {

std::shared_ptr<BaseExpression>
InsertBinder::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext> &bind_context_ptr) {
    std::shared_ptr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    return result;
}

std::shared_ptr<BaseExpression>
InsertBinder::BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("HavingBinder::BuildColRefExpr");
}

}