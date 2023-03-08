//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "bind_context.h"

#include "expression/base_expression.h"
#include "expression/subquery_expression.h"
#include "function/function.h"
#include "parser/expression.h"

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

    SharedPtr<BaseExpression>
    Bind(const ParsedExpr& expr,
         BindContext* bind_context_ptr,
         i64 depth,
         bool root);

    // Bind expression entry
    virtual SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    BindContext* bind_context_ptr,
                    i64 depth,
                    bool root);

    virtual SharedPtr<BaseExpression>
    BuildValueExpr(const ConstantExpr& expr,
                   BindContext* bind_context_ptr,
                   i64 depth,
                   bool root);

    // Bind column reference expression also include correlated column reference.
    virtual SharedPtr<BaseExpression>
    BuildColExpr(const ColumnExpr& expr,
                 BindContext* bind_context_ptr,
                 i64 depth,
                 bool root);

    virtual SharedPtr<BaseExpression>
    BuildFuncExpr(const FunctionExpr& expr,
                  BindContext* bind_context_ptr,
                  i64 depth,
                  bool root);

    virtual void
    CheckFuncType(FunctionType func_type) const {}

    virtual SharedPtr<BaseExpression>
    BuildCastExpr(const CastExpr& expr,
                  BindContext* bind_context_ptr,
                  i64 depth,
                  bool root);

    virtual SharedPtr<BaseExpression>
    BuildCaseExpr(const CaseExpr& expr,
                  BindContext* bind_context_ptr,
                  i64 depth,
                  bool root);

    // Bind subquery expression.
    virtual SharedPtr<SubqueryExpression>
    BuildSubquery(const SubqueryExpr& expr,
                  BindContext* bind_context_ptr,
                  SubqueryType subquery_type,
                  i64 depth,
                  bool root);

//    // Bind window function.
//    virtual SharedPtr<BaseExpression>
//    BuildWindow(const hsql::Expr &expr, const SharedPtr<BindContext>& bind_context_ptr);

//    SharedPtr<PlanBuilder> plan_builder_ptr_;
protected:
    SharedPtr<QueryContext>& query_context_;

};

}
