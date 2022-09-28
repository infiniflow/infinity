//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"

namespace infinity {

class HavingBinder : public ExpressionBinder {
public:
    explicit HavingBinder(std::shared_ptr<QueryContext>& query_context,
                          const std::shared_ptr<BindAliasProxy>& bind_alias_proxy)
                          : ExpressionBinder(query_context),
                          bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

//    // Bind column reference expression also include correlated column reference.
//    std::shared_ptr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;
private:

    const std::shared_ptr<BindAliasProxy>& bind_alias_proxy_;
    bool binding_agg_func_ = false;
};

}
