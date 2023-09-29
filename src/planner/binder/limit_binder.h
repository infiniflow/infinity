//
// Created by JinHai on 2022/8/13.
//

#pragma once

#include "planner/expression_binder.h"
#include "function/function.h"

namespace infinity {

class LimitBinder final : public ExpressionBinder {
public:
    explicit LimitBinder(QueryContext* query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    BindContext* bind_context_ptr,
                    i64 depth,
                    bool root) final;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const FunctionExpr& expr,
                  BindContext* bind_context_ptr,
                  i64 depth,
                  bool root) final;

    SharedPtr<BaseExpression>
    BuildColExpr(const ColumnExpr& expr,
                 BindContext* bind_context_ptr,
                 i64 depth,
                 bool root) final;


    SharedPtr<BaseExpression>
    BuildKnnExpr(const KnnExpr& expr,
                 BindContext* bind_context_ptr,
                 i64 depth,
                 bool root) final;

private:
};

}
