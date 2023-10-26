//
// Created by JinHai on 2022/8/12.
//

module;

import stl;
import base_expression;
import parser;
import function;
import bind_context;
import infinity_assert;
import infinity_exception;
import third_party;
import bind_alias_proxy;

module where_binder;

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

    if (result.get() != nullptr) {
        return result;
    }

    // The column isn't found, check if it is an alias of select list expression
    if (bind_alias_proxy_.get() != nullptr) {
        // Yes, it is an alias from select list expression
        // Extract the expression, and bind it during binding where clause phase .
        result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr, depth, root);
    }

    Assert<PlannerException>(result.get() != nullptr, Format("Can't bind the expr: {}", expr.GetName()), __FILE_NAME__, __LINE__);
    return result;
}

void WhereBinder::CheckFuncType(FunctionType func_type) const {
    Assert<PlannerException>(func_type == FunctionType::kScalar, "Only scalar function are allowed in where clause", __FILE_NAME__, __LINE__);
}

} // namespace infinity