//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"

namespace infinity {

class HavingBinder : public ExpressionBinder {
public:
    explicit
    HavingBinder(SharedPtr<QueryContext>& query_context,
                          const SharedPtr<BindAliasProxy>& bind_alias_proxy)
                          : ExpressionBinder(query_context),
                          bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr,
                    const SharedPtr<BindContext>& bind_context_ptr,
                    i64 depth,
                    bool root) override;

    SharedPtr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr,
                 const SharedPtr<BindContext>& bind_context_ptr,
                 i64 depth,
                 bool root) override;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr,
                  const SharedPtr<BindContext>& bind_context_ptr,
                  i64 depth,
                  bool root) override;

//    // Bind column reference expression also include correlated column reference.
//    SharedPtr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;
private:

    const SharedPtr<BindAliasProxy>& bind_alias_proxy_;
    bool binding_agg_func_ = false;
};

}
