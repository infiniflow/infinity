//
// Created by JinHai on 2022/9/24.
//

#include "bind_alias_proxy.h"
#include "common/utility/infinity_assert.h"
#include "parser/expression/parsed_raw_expression.h"
#include "parser/statement.h"

namespace infinity {

SharedPtr<BaseExpression>
BindAliasProxy::BindAlias(ExpressionBinder& expression_binder,
                          const hsql::Expr& expr,
                          const SharedPtr<BindContext>& bind_context_ptr) {
    String expr_name = expr.getName() == nullptr ? Statement::ExprAsColumnName(&expr) : expr.getName();

    auto alias_pair = alias2expr_.find(expr_name);
    if(alias_pair == alias2expr_.end()) {
        return nullptr;
    }

    if(binding_alias_) {
        PlannerError("Trying to bind an alias name: " + expr_name + " in another alias");
    }

    SharedPtr<BaseExpression> bound_alias_expr = nullptr;
    binding_alias_ = true;
    if(alias_pair->second->type_ == ExpressionType::kRaw) {
        SharedPtr<ParsedRawExpression> parsed_raw_expr
                = std::static_pointer_cast<ParsedRawExpression>(alias_pair->second);
        bound_alias_expr = expression_binder.BuildExpression(*parsed_raw_expr->raw_expr_, bind_context_ptr);
    } else {
        SharedPtr<ParsedColumnExpression> parsed_col_expr
                = std::static_pointer_cast<ParsedColumnExpression>(alias_pair->second);
        bound_alias_expr = expression_binder.BuildColExpr(parsed_col_expr, bind_context_ptr);
    }
    binding_alias_ = false;

    return bound_alias_expr;
#if 0
    String expr_name = expr.getName();

    auto bound_expr_pair = bind_context_ptr->project_by_name_.find(expr_name);
    if(bound_expr_pair != bind_context_ptr->project_by_name_.end()) {
        return bound_expr_pair->second;
    }

    auto alias_pair = alias2expr_.find(expr_name);
    if(alias_pair == alias2expr_.end()) {
        return nullptr;
    }

    if(binding_alias_) {
        PlannerError("Trying to bind an alias name: " + expr_name + " in another alias");
    }

    binding_alias_ = true;
    auto bound_alias_expr = expression_binder.BuildExpression(*alias_pair->second, bind_context_ptr);
    binding_alias_ = false;

    // Store the relationship into bind context
    bind_context_ptr->project_by_name_.emplace(expr_name, bound_alias_expr);
    return bound_alias_expr;
#endif
}

}
