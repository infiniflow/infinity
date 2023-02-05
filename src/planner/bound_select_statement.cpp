//
// Created by jinhai on 23-1-22.
//
#include "main/infinity.h"
#include "bound_select_statement.h"
#include "bound/subquery_unnest.h"
#include "bound/base_table_ref.h"
#include "bound/join_table_ref.h"
#include "bound/subquery_table_ref.h"
#include "bound/cross_product_table_ref.h"
#include "bound/dummy_table_ref.h"

#include "common/utility/infinity_assert.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_join.h"
#include "planner/node/logical_limit.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_filter.h"
#include "planner/node/logical_dummy_scan.h"
#include "expression/expression_transformer.h"
#include "planner/node/logical_sort.h"
#include "planner/node/logical_aggregate.h"

namespace infinity {

SharedPtr<LogicalNode>
BoundSelectStatement::BuildPlan() {

    SharedPtr<LogicalNode> root = BuildFrom(table_ref_ptr_);
    if(!where_conditions_.empty()) {
        SharedPtr<LogicalNode> filter = BuildFilter(root, where_conditions_);
        filter->set_left_node(root);
        root = filter;
    }

    if(!group_by_expressions_.empty() || !aggregate_expressions_.empty()) {
        // Build logical aggregate
        auto aggregate = MakeShared<LogicalAggregate>(group_by_expressions_,
                                                      groupby_index_,
                                                      aggregate_expressions_,
                                                      aggregate_index_);
        aggregate->set_left_node(root);
        root = aggregate;
    }

    if(!having_expressions_.empty()) {
        // Build logical filter
        auto having_filter = BuildFilter(root, having_expressions_);
        having_filter->set_left_node(root);
        root = having_filter;
    }

    auto project = MakeShared<LogicalProject>(projection_expressions_, projection_index_);
    project->set_left_node(root);
    root = project;

    if(!order_by_expressions_.empty()) {
        PlannerAssert(order_by_expressions_.size() == order_by_types_.size(), "Unknown error on order by expression");
        SharedPtr<LogicalNode> sort
                = MakeShared<LogicalSort>(order_by_expressions_, order_by_types_);
        sort->set_left_node(root);
        root = sort;
    }

    if(limit_expression_ != nullptr) {
        SharedPtr<LogicalLimit> limit
                = MakeShared<LogicalLimit>(limit_expression_, offset_expression_);
        limit->set_left_node(root);
        root = limit;
    }

    if(!pruned_expression_.empty()) {
        auto pruned_project = MakeShared<LogicalProject>(pruned_expression_, result_index_);
        pruned_project->set_left_node(root);
        root = pruned_project;
    }

    return root;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildFrom(SharedPtr<TableRef>& table_ref) {
    if(table_ref) {
        switch(table_ref->type_) {
            case TableRefType::kBaseTable: {
                return BuildBaseTable(table_ref);
            }
            case TableRefType::kSubquery: {
                return BuildSubqueryTable(table_ref);
            }
            case TableRefType::kCrossProduct: {
                return BuildCrossProductTable(table_ref);
            }
            case TableRefType::kJoin: {
                return BuildJoinTable(table_ref);
            }
            case TableRefType::kDummy: {
                return BuildDummyTable(table_ref);
            }
            default:
                PlannerError("Unknown table reference type.");
        }
    } else {
        // No from clause, only select constant value is OK ?
    }

    return nullptr;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildBaseTable(SharedPtr<TableRef>& table_ref) {
    // SharedPtr<BaseTableRef> base_table_ref
    auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref);

    SharedPtr<LogicalTableScan> table_scan_node
            = MakeShared<LogicalTableScan>(base_table_ref->table_scan_function_data_->table_ptr_,
                                                 base_table_ref->table_scan_func_,
                                                 base_table_ref->alias_,
                                                 base_table_ref->column_names_,
                                                 base_table_ref->column_types_);
    return table_scan_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildSubqueryTable(SharedPtr<TableRef>& table_ref) {
    // SharedPtr<SubqueryTableRef> subquery_table_ref
    auto subquery_table_ref = std::static_pointer_cast<SubqueryTableRef>(table_ref);
    SharedPtr<LogicalNode> subquery = subquery_table_ref->subquery_node_->logical_plan_;
    return subquery;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildCrossProductTable(SharedPtr<TableRef>& table_ref) {
    // SharedPtr<CrossProductTableRef> cross_product_table_ref
    auto cross_product_table_ref = std::static_pointer_cast<CrossProductTableRef>(table_ref);

    auto left_node = BuildFrom(cross_product_table_ref->left_table_ref_);
    auto right_node = BuildFrom(cross_product_table_ref->right_table_ref_);

    // TODO: Merge bind context ?

    SharedPtr<LogicalCrossProduct> logical_cross_product_node =
            MakeShared<LogicalCrossProduct>(left_node, right_node);
    return logical_cross_product_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildJoinTable(SharedPtr<TableRef>& table_ref) {
    // SharedPtr<JoinTableRef> join_table_ref
    auto join_table_ref = std::static_pointer_cast<JoinTableRef>(table_ref);

    auto left_node = BuildFrom(join_table_ref->left_table_ref_);
    auto right_node = BuildFrom(join_table_ref->right_table_ref_);

    // TODO: Merge bind context ?

    SharedPtr<LogicalJoin> logical_join_node =
            MakeShared<LogicalJoin>(join_table_ref->join_type_, join_table_ref->on_conditions_,
                                          left_node, right_node);
    return logical_join_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildDummyTable(SharedPtr<TableRef>& table_ref) {
//    auto dummy_table_ref = std::static_pointer_cast<DummyTableRef>(table_ref);
    SharedPtr<LogicalDummyScan> dummy_scan_node = MakeShared<LogicalDummyScan>();
    return dummy_scan_node;
}

SharedPtr<LogicalNode>
BoundSelectStatement::BuildFilter(SharedPtr<LogicalNode> root,
                                  Vector<SharedPtr<BaseExpression>>& conditions) {
    for(auto& cond: conditions) {
        // 1. Go through all the expression to find subquery
        VisitExpression(cond,
                        [&](SharedPtr<BaseExpression> &expr) {
                            SubqueryUnnest::UnnestSubqueries(expr, root);
                        });
    }

    // SharedPtr<BaseExpression> filter_expr
    auto filter_expr = ComposeExpressionWithDelimiter(conditions, ConjunctionType::kAnd);

    // SharedPtr<LogicalFilter> filter
    auto filter = MakeShared<LogicalFilter>(filter_expr);

    return filter;
}

}
