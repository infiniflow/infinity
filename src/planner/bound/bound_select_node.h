//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "planner/logical_node.h"
#include "planner/node/logical_sort.h"
#include "planner/bound_node.h"
#include "expression/column_expression.h"
#include "planner/bind_context.h"

#include "sql/Expr.h"
#include "table_ref.h"

namespace infinity {

class BoundSelectNode : public BoundNode {
public:
    BoundSelectNode(SharedPtr<BindContext> bind_context_ptr)
        : BoundNode(BoundNodeType::kSelect), bind_context_ptr_(std::move(bind_context_ptr)) {}

    SharedPtr<LogicalNode>
    BuildPlan() override;

    SharedPtr<LogicalNode>
    BuildFrom(SharedPtr<TableRef>& table_ref, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<LogicalNode>
    BuildBaseTable(SharedPtr<TableRef>& table_ref, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<LogicalNode>
    BuildSubqueryTable(SharedPtr<TableRef>& table_ref, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<LogicalNode>
    BuildCrossProductTable(SharedPtr<TableRef>& table_ref, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<LogicalNode>
    BuildJoinTable(SharedPtr<TableRef>& table_ref, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<LogicalNode>
    BuildDummyTable(SharedPtr<TableRef>& table_ref, SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<LogicalNode>
    BuildFilter(SharedPtr<LogicalNode> root,
                Vector<SharedPtr<BaseExpression>>& conditions,
                SharedPtr<BindContext>& bind_context_ptr);

    SharedPtr<BindContext> bind_context_ptr_;

    // From clause
    SharedPtr<TableRef> table_ref_ptr_;

    // Where conditions
    Vector<SharedPtr<BaseExpression>> where_conditions_;

    // Group by expression list
    Vector<SharedPtr<BaseExpression>> group_by_expressions_;

    // Having expression list
    Vector<SharedPtr<BaseExpression>> having_expressions_;

    // Project expression list
    Vector<SharedPtr<BaseExpression>> projection_expressions_;

    // Order by expression list
    Vector<SharedPtr<BaseExpression>> order_by_expressions_;
    Vector<OrderByType> order_by_types_;

    // limit expression
    SharedPtr<BaseExpression> limit_expression_;
    SharedPtr<BaseExpression> offset_expression_;

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

    explicit SelectItem(SharedPtr<ColumnExpression> column_expr)
            : column_expr_(column_expr), type_(SelectItemType::kBoundColumnExpr)
            {}

    SelectItemType type_{SelectItemType::kInvalid};
    const hsql::Expr* expr_; // Unbound expression
    SharedPtr<ColumnExpression> column_expr_; // Bound Column expression
};

}
