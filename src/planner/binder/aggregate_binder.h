//
// Created by JinHai on 2022/8/12.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class AggregateBinder : public ExpressionBinder {
public:
//    explicit AggregateBinder(PlanBuilder& plan_builder) : ExpressionBinder(plan_builder) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    const SharedPtr<BindContext>& bind_context_ptr,
                    i64 depth,
                    bool root) override;
//
//    // Bind column reference expression also include correlated column reference.
//    SharedPtr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr) override;
private:
};

}
