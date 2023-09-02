//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"
#include "bind_alias_proxy.h"

namespace infinity {

class HavingBinder final: public ExpressionBinder {
public:
    explicit
    HavingBinder(QueryContext* query_context,
                          const SharedPtr<BindAliasProxy>& bind_alias_proxy)
                          : ExpressionBinder(query_context),
                          bind_alias_proxy_(bind_alias_proxy) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    BindContext* bind_context_ptr,
                    i64 depth,
                    bool root) final;

    SharedPtr<BaseExpression>
    BuildColExpr(const ColumnExpr& expr,
                 BindContext* bind_context_ptr,
                 i64 depth,
                 bool root) final;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const FunctionExpr& expr,
                  BindContext* bind_context_ptr,
                  i64 depth,
                  bool root) final;

//    // Bind column reference expression also include correlated column reference.
//    SharedPtr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, BindContext* bind_context_ptr) override;
private:

    const SharedPtr<BindAliasProxy>& bind_alias_proxy_;
    bool binding_agg_func_ = false;
};

}
