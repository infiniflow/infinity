//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class JoinBinder final : public ExpressionBinder {
public:
    explicit
    JoinBinder(QueryContext* query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    BindContext* bind_context_ptr,
                    i64 depth,
                    bool root) final;

//    // Bind column reference expression also include correlated column reference.
//    SharedPtr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;
private:
};

}

