//
// Created by JinHai on 2022/8/12.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class ProjectBinder : public ExpressionBinder {
public:
    explicit
    ProjectBinder(SharedPtr<QueryContext>& query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    SharedPtr<BaseExpression>
    BuildExpression(const hsql::Expr &expr,
                    const SharedPtr<BindContext>& bind_context_ptr,
                    i64 depth,
                    bool root) override;

    SharedPtr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr,
                  const SharedPtr<BindContext>& bind_context_ptr,
                  i64 depth,
                  bool root) override;

    SharedPtr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr,
                 const SharedPtr<BindContext>& bind_context_ptr,
                 i64 depth,
                 bool root) override;

    [[nodiscard]] const String&
    BoundColumn() const { return bound_column_name_; }
private:
    bool binding_agg_func_ = false;
    String bound_column_name_;
};

}

