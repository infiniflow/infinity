//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "expression/base_expression.h"
#include "bind_context.h"

#include <memory>

namespace infinity {

class ExpressionBinder {
public:
    ExpressionBinder() = default;
    virtual ~ExpressionBinder() = default;

    // Bind expression entry
    virtual std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Bind column reference expression also include correlated column reference.
    virtual std::shared_ptr<BaseExpression>
    BuildColRefExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Bind aggregate function.
    virtual std::shared_ptr<BaseExpression>
    BuildAggFunc(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);
private:
};

}
