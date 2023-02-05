//
// Created by JinHai on 2022/8/13.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class OrderBinder : public ExpressionBinder {
public:
    static void
    PushExtraExprToSelectList(const hsql::Expr *expr,
                              const SharedPtr<BindContext>& bind_context_ptr);

public:
    explicit OrderBinder(SharedPtr<QueryContext>& query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr,
                    const SharedPtr<BindContext>& bind_context_ptr,
                    i64 depth,
                    bool root) override;
private:
};

}
