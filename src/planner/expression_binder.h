//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "bind_context.h"

#include "expression/base_expression.h"
#include "expression/subquery_expression.h"
#include "function/function.h"
#include "parser/parsed_expression.h"

#include <memory>

namespace infinity {

struct ExprBindResult {
    SharedPtr<BaseExpression> bound_expression_ptr_{};
    String err_msg_{};

    inline bool IsOk() { return err_msg_.empty(); }
};


class ExpressionBinder {
public:
    explicit
    ExpressionBinder(SharedPtr<QueryContext>& query_context) : query_context_(query_context) {}

    virtual
    ~ExpressionBinder() = default;

    // Bind expression entry
    virtual SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    virtual SharedPtr<BaseExpression>
    BuildValueExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    // Bind column reference expression also include correlated column reference.
    virtual SharedPtr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<BaseExpression>
    BuildColExpr(const SharedPtr<ParsedColumnExpression>& expr, const SharedPtr<BindContext>& bind_context_ptr);

    virtual SharedPtr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    virtual void
    CheckFuncType(FunctionType func_type) const {}

    virtual SharedPtr<BaseExpression>
    BuildOperatorExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    virtual SharedPtr<BaseExpression>
    BuildCastExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    virtual SharedPtr<BaseExpression>
    BuildCaseExpr(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

    virtual SharedPtr<BaseExpression>
    BuildBinaryScalarExpr(String& op,
                          const hsql::Expr* left,
                          const hsql::Expr* right,
                          const SharedPtr<BindContext>& bind_context_ptr);

    virtual SharedPtr<BaseExpression>
    BuildUnaryScalarExpr(String& op,
                         const hsql::Expr* expr,
                         const SharedPtr<BindContext>& bind_context_ptr);

    // Bind subquery expression.
    virtual SharedPtr<SubqueryExpression>
    BuildSubquery(const hsql::SelectStatement& select,
                  const SharedPtr<BindContext>& bind_context_ptr, SubqueryType subquery_type);

//    // Bind window function.
//    virtual SharedPtr<BaseExpression>
//    BuildWindow(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

//    SharedPtr<PlanBuilder> plan_builder_ptr_;
protected:
    SharedPtr<QueryContext>& query_context_;

};

}
