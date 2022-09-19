//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class JoinBinder : public ExpressionBinder {
public:
//    explicit JoinBinder(PlanBuilder& plan_builder) : ExpressionBinder(plan_builder) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

//    // Bind column reference expression also include correlated column reference.
//    std::shared_ptr<BaseExpression>
//    BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;
private:
};

}

