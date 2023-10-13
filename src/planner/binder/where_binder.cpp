//
// Created by JinHai on 2022/8/12.
//

#include "planner/binder/bind_alias_proxy.h"
#include "planner/binder/where_binder.h"

namespace infinity {

SharedPtr<BaseExpression> WhereBinder::BuildExpression(const ParsedExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr, depth, root);
    return result;
}

SharedPtr<BaseExpression> WhereBinder::BuildColExpr(const ColumnExpr &expr, BindContext *bind_context_ptr, i64 depth, bool root) {
    // Step 1. Trying to bind the column to current binding. (done)
    // Step 2. Trying to bind the column to current select list alias. (done)

    // Use expression binder to check the column firstly
    auto result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr, depth, root);

    if (result != nullptr) {
        return result;
    }

    // The column isn't found, check if it is an alias of select list expression
    if (bind_alias_proxy_ != nullptr) {
        // Yes, it is an alias from select list expression
        // Extract the expression, and bind it during binding where clause phase .
        result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr, depth, root);
    }

    PlannerAssert(result != nullptr, String("Can't bind the expr: ") + expr.GetName());
    return result;
}

void WhereBinder::CheckFuncType(FunctionType func_type) const {
    PlannerAssert(func_type == FunctionType::kScalar, "Only scalar function are allowed in where clause");
}

} // namespace infinity