//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "planner/bound_node.h"
#include "expression/column_expression.h"
#include "sql/Expr.h"

namespace infinity {

class TableRef;
class BindContext;

class BoundSelectNode : public BoundNode {
public:
    BoundSelectNode(std::shared_ptr<BindContext> bind_context_ptr)
        : BoundNode(BoundNodeType::kSelect), bind_context_ptr_(std::move(bind_context_ptr)) {}

    std::shared_ptr<BindContext> bind_context_ptr_;

    // From clause
    std::shared_ptr<TableRef> table_ref_ptr_;

    // Group by expression list
    std::vector<std::shared_ptr<BaseExpression>> group_by_expressions_;

    // Having expression list
    std::vector<std::shared_ptr<BaseExpression>> having_expressions_;

    // Project expression list
    std::vector<std::shared_ptr<BaseExpression>> projection_expressions_;

    int64_t GetTableIndex() override { return 0; };

};

enum class SelectItemType {
    kInvalid,
    kRawExpr,
    kBoundColumnExpr,
};

struct SelectItem {
    explicit SelectItem(const hsql::Expr* expr)
        : expr_(expr), type_(SelectItemType::kRawExpr)
        {}

    explicit SelectItem(std::shared_ptr<ColumnExpression> column_expr)
            : column_expr_(column_expr), type_(SelectItemType::kBoundColumnExpr)
            {}

    SelectItemType type_{SelectItemType::kInvalid};
    const hsql::Expr* expr_; // Unbound expression
    std::shared_ptr<ColumnExpression> column_expr_; // Bound Column expression
};

}
