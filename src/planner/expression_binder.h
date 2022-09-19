//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "expression/base_expression.h"
#include "bind_context.h"

#include <memory>

namespace infinity {
//class PlanBuilder;

class ExpressionBinder {
public:
//    explicit ExpressionBinder(std::shared_ptr<PlanBuilder> plan_builder_ptr)
//        : plan_builder_ptr_(std::move(plan_builder_ptr)) {};
    virtual ~ExpressionBinder() = default;

    // Bind expression entry
    virtual std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Bind column reference expression also include correlated column reference.
    std::shared_ptr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildOperatorExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildCastExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildCaseExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildBinaryScalarExpr(const std::string& op, const hsql::Expr* left, const hsql::Expr* right, const std::shared_ptr<BindContext>& bind_context_ptr);

    std::shared_ptr<BaseExpression>
    BuildUnaryScalarExpr(const std::string& op, const hsql::Expr* expr, const std::shared_ptr<BindContext>& bind_context_ptr);

    // Bind subquery expression.
    std::shared_ptr<BaseExpression>
    BuildSubquery(const hsql::SelectStatement& select, const std::shared_ptr<BindContext>& bind_context_ptr);

//    // Bind window function.
//    virtual std::shared_ptr<BaseExpression>
//    BuildWindow(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr);

//    std::shared_ptr<PlanBuilder> plan_builder_ptr_;
private:
};

}
