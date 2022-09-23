//
// Created by JinHai on 2022/8/11.
//

#pragma once

#include "planner/expression_binder.h"

namespace infinity {

class GroupBinder : public ExpressionBinder {
public:
    // Bind expression entry
    std::shared_ptr<BaseExpression>
    BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<BaseExpression>
    BuildFuncExpr(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) override;

    std::shared_ptr<SubqueryExpression>
    BuildSubquery(const hsql::SelectStatement& select, const std::shared_ptr<BindContext>& bind_context_ptr, SubqueryType subquery_type) override;
private:
};

}

