//
// Created by JinHai on 2022/9/24.
//

#include "bind_alias_proxy.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

std::shared_ptr<BaseExpression>
BindAliasProxy::BindAlias(ExpressionBinder& expression_binder, const hsql::Expr& expr, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::string expr_name = expr.getName();

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
}

}
