//
// Created by JinHai on 2022/8/12.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"

namespace infinity {

class WhereBinder : public ExpressionBinder {
public:
    explicit WhereBinder(std::shared_ptr<QueryContext>& query_context,
                         const std::shared_ptr<BindAliasProxy>& bind_alias_proxy)
        : ExpressionBinder(query_context),
        bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;
private:
    const std::shared_ptr<BindAliasProxy>& bind_alias_proxy_;
};

}
