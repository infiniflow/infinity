//
// Created by JinHai on 2022/9/24.
//

#include "bind_alias_proxy.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

SharedPtr<BaseExpression>
BindAliasProxy::BindAlias(ExpressionBinder& expression_binder,
                          const ParsedExpr& expr,
                          BindContext* bind_context_ptr,
                          i64 depth,
                          bool root) {
    String expr_name = expr.GetName();

    auto alias_pair = bind_context_ptr->select_alias2index_.find(expr_name);
    if(alias_pair == bind_context_ptr->select_alias2index_.end()) {
        return nullptr;
    }

    const ParsedExpr* select_expr = bind_context_ptr->select_expression_[alias_pair->second];

    if(binding_alias_) {
        PlannerError("Trying to bind an alias table_name: " + expr_name + " in another alias");
    }

    binding_alias_ = true;
    SharedPtr<BaseExpression> bound_alias_expr = expression_binder.BuildExpression(*select_expr,
                                                                                   bind_context_ptr,
                                                                                   depth,
                                                                                   root);
    binding_alias_ = false;

    return bound_alias_expr;
}

}
