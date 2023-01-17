//
// Created by JinHai on 2022/8/13.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class OrderBinder : public ExpressionBinder {
public:
    explicit OrderBinder(SharedPtr<QueryContext>& query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    SharedPtr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;
private:
};

}
