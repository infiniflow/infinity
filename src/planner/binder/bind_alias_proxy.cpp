//
// Created by JinHai on 2022/9/24.
//

#include "bind_alias_proxy.h"
#include "common/utility/infinity_assert.h"
#include "legacy_parser/expression/parsed_raw_expression.h"
#include "legacy_parser/statement.h"

namespace infinity {

SharedPtr<BaseExpression>
BindAliasProxy::BindAlias(ExpressionBinder& expression_binder,
                          const hsql::Expr& expr,
                          const SharedPtr<BindContext>& bind_context_ptr,
                          i64 depth,
                          bool root) {
    String expr_name = expr.getName() == nullptr ? Statement::ExprAsColumnName(&expr) : expr.getName();

    auto alias_pair = bind_context_ptr->select_alias2index_.find(expr_name);
    if(alias_pair == bind_context_ptr->select_alias2index_.end()) {
        return nullptr;
    }

    const SharedPtr<ParsedExpression>& select_expr = bind_context_ptr->select_expression_[alias_pair->second];

    if(binding_alias_) {
        PlannerError("Trying to bind an alias name: " + expr_name + " in another alias");
    }

    SharedPtr<BaseExpression> bound_alias_expr = nullptr;
    binding_alias_ = true;
    if(select_expr->type_ == ExpressionType::kRaw) {
        SharedPtr<ParsedRawExpression> parsed_raw_expr
                = std::static_pointer_cast<ParsedRawExpression>(select_expr);
        bound_alias_expr = expression_binder.BuildExpression(*parsed_raw_expr->raw_expr_,
                                                             bind_context_ptr,
                                                             depth,
                                                             root);
    } else {
        SharedPtr<ParsedColumnExpression> parsed_col_expr
                = std::static_pointer_cast<ParsedColumnExpression>(select_expr);
        bound_alias_expr = expression_binder.BuildColExpr(parsed_col_expr,
                                                          bind_context_ptr,
                                                          depth,
                                                          root);
    }
    binding_alias_ = false;

    return bound_alias_expr;
}

}
