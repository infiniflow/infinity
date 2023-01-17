//
// Created by JinHai on 2022/8/12.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"
#include "function/function.h"

namespace infinity {

class WhereBinder : public ExpressionBinder {
public:
    explicit
    WhereBinder(SharedPtr<QueryContext>& query_context,
                         const SharedPtr<BindAliasProxy>& bind_alias_proxy)
        : ExpressionBinder(query_context),
        bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    SharedPtr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    void
    CheckFuncType(FunctionType func_type) const override;
private:
    const SharedPtr<BindAliasProxy>& bind_alias_proxy_;
};

}
