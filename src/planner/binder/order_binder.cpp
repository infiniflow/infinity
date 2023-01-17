//
// Created by JinHai on 2022/8/13.
//

#include "order_binder.h"
#include "function/function_set.h"
#include "parser/statement.h"

namespace infinity {

SharedPtr<BaseExpression>
OrderBinder::BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext> &bind_context_ptr) {
    if(expr.type == hsql::kExprLiteralInt) {
        i64 column_id = expr.ival;
        if(column_id >= bind_context_ptr->project_names_.size()) {
            PlannerError("Order by are going to use nonexistent column from select list.")
        }
        const String& column_name_of_project = bind_context_ptr->project_names_[column_id];
        return bind_context_ptr->project_by_name_[column_name_of_project];
    }

    SharedPtr<BaseExpression> result = ExpressionBinder::BuildExpression(expr, bind_context_ptr);
    return result;
}

SharedPtr<BaseExpression>
OrderBinder::BuildColExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
    // Order by related column must come from project list
    String expr_name = expr.getName() == nullptr ? Statement::ExprAsColumnName(&expr) : expr.getName();

    auto bound_expr_ptr = bind_context_ptr->project_by_name_[expr_name];

    if(bound_expr_ptr) {
        return bound_expr_ptr;
    }

    // Not found in current project list
    bound_expr_ptr = ExpressionBinder::BuildColExpr(expr, bind_context_ptr);

    // Store the new bound expression into project list;
    bind_context_ptr->project_by_name_[expr_name] = bound_expr_ptr;
    bind_context_ptr->project_names_.emplace_back(expr_name);

    return bound_expr_ptr;
}

SharedPtr<BaseExpression>
OrderBinder::BuildFuncExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) {
    SharedPtr<FunctionSet> function_set_ptr = FunctionSet::GetFunctionSet(expr);
    if(function_set_ptr->type_ != FunctionType::kScalar) {
        PlannerError("Only scalar function is supported in order by list.");
    }
    return ExpressionBinder::BuildFuncExpr(expr, bind_context_ptr);
}

}
