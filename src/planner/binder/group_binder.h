//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"

namespace infinity {

class GroupBinder : public ExpressionBinder {
public:
    explicit GroupBinder(SharedPtr<QueryContext>& query_context,
                         const SharedPtr<BindAliasProxy>& bind_alias_proxy)
                         : ExpressionBinder(query_context),
                         bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    SharedPtr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    void
    CheckFuncType(FunctionType func_type) const override;

    SharedPtr<SubqueryExpression>
    BuildSubquery(const hsql::SelectStatement& select, const SharedPtr<BindContext>& bind_context_ptr, SubqueryType subquery_type) override;
private:

    const SharedPtr<BindAliasProxy>& bind_alias_proxy_;
};

}

