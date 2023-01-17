//
// Created by JinHai on 2022/8/11.
//

#include "common/utility/infinity_assert.h"
#include "function/function_set.h"
#include "group_binder.h"
#include "parser/statement.h"

namespace infinity {

SharedPtr<BaseExpression>
GroupBinder::BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);

    // if(root_expression) {  } why we need root expression flag?
    {
        String expr_name = expr.getName() == nullptr ? Statement::ExprAsColumnName(&expr) : expr.getName();

        if(bind_context_ptr->group_by_name_.contains(expr_name)) {
            PlannerError("Duplicated group by expression: " + expr_name);
        }

        // Add the group by expression into bind context
        bind_context_ptr->group_names_.emplace_back(expr_name);
        bind_context_ptr->group_by_name_[expr_name] = result;
    }

    return result;
}

SharedPtr<BaseExpression>
GroupBinder::BuildColExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {

    // Check if the column is using an alias from select list.
    auto result = bind_alias_proxy_->BindAlias(*this, expr, bind_context_ptr);

    if(result == nullptr) {
        result = ExpressionBinder::BuildColExpr(expr, bind_context_ptr);
    }

    return result;
}

SharedPtr<BaseExpression>
GroupBinder::BuildFuncExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ != FunctionType::kScalar) {
        PlannerError("Only scalar function is supported in group by list.");
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr);
}

void
GroupBinder::CheckFuncType(FunctionType func_type) const {
    PlannerAssert(func_type == FunctionType::kScalar, "Onlyl scalar function type are allowed in group by clause");
}

SharedPtr<SubqueryExpression>
GroupBinder::BuildSubquery(const hsql::SelectStatement& select, const SharedPtr<BindContext>& bind_context_ptr, SubqueryType subquery_type) {
    PlannerError("Subquery isn't supported in group by list.");
}

}
