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
    std::shared_ptr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);
//
//    // Bind aggregate function.
//    virtual std::shared_ptr<BaseExpression>
//    BuildAggFunc(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);
//
//    // Bind subquery expression.
//    virtual std::shared_ptr<BaseExpression>
//    BuildSubquery(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);
//
//    // Bind window function.
//    virtual std::shared_ptr<BaseExpression>
//    BuildWindow(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

private:
};

}
