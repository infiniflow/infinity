//
// Created by JinHai on 2022/8/12.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class ProjectBinder : public ExpressionBinder {
public:
    explicit ProjectBinder(std::shared_ptr<QueryContext>& query_context) : ExpressionBinder(query_context) {}

    // Bind expression entry
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression>
    BuildColExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;


    [[nodiscard]] const std::string& BoundColumn() const { return bound_column_name_; }
private:
    bool binding_agg_func_ = false;
    std::string bound_column_name_;
};

}

