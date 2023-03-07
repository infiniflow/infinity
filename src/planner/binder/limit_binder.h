//
// Created by JinHai on 2022/8/13.
//

#pragma once

#include "planner/expression_binder.h"
#include "function/function.h"

namespace infinity {

class LimitBinder final : public ExpressionBinder {
public:
    explicit LimitBinder(SharedPtr<QueryContext>& query_context): ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const ParsedExpr& expr,
                    const SharedPtr<BindContext>& bind_context_ptr,
                    i64 depth,
                    bool root) final ;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const FunctionExpr& expr,
                  const SharedPtr<BindContext>& bind_context_ptr,
                  i64 depth,
                  bool root) final;

    SharedPtr<BaseExpression>
    BuildColExpr(const ColumnExpr& expr,
                 const SharedPtr<BindContext>& bind_context_ptr,
                 i64 depth,
                 bool root) final;

private:
};

}
