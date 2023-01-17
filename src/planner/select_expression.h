//
// Created by jinhai on 23-1-20.
//

#pragma once
#include "common/types/internal_types.h"
#include "expression/column_expression.h"
#include "sql/Expr.h"

namespace infinity {

enum class SelectItemType {
    kInvalid,
    kRawExpr,
    kBoundColumnExpr,
};

struct SelectExpression {
    explicit
    SelectExpression(const hsql::Expr* expr)
            : expr_(expr), type_(SelectItemType::kRawExpr)
    {}

    explicit
    SelectExpression(SharedPtr<ColumnExpression> column_expr)
            : column_expr_(std::move(column_expr)), type_(SelectItemType::kBoundColumnExpr)
    {}

    SelectItemType type_{SelectItemType::kInvalid};
    const hsql::Expr* expr_{}; // Unbound expression
    SharedPtr<ColumnExpression> column_expr_{}; // Bound Column expression
};

}


