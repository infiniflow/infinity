//
// Created by JinHai on 2022/9/12.
//

#include "main/infinity.h"
#include "bound_select_node.h"
#include "subquery_unnest.h"
#include "base_table_ref.h"
#include "join_table_ref.h"
#include "subquery_table_ref.h"
#include "cross_product_table_ref.h"
#include "dummy_table_ref.h"

#include "common/utility/infinity_assert.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_join.h"
#include "planner/node/logical_limit.h"
#include "planner/node/logical_project.h"
#include "planner/node/logical_filter.h"
#include "planner/node/logical_dummy_scan.h"
#include "expression/expression_transformer.h"

namespace infinity {
std::shared_ptr<LogicalNode>
BoundSelectNode::BuildPlan() {

    std::shared_ptr<LogicalNode> root = BuildFrom(table_ref_ptr_, bind_context_ptr_);
    if(!where_conditions_.empty()) {
        std::shared_ptr<LogicalNode> filter = BuildFilter(root, where_conditions_, bind_context_ptr_);
        filter->set_left_node(root);
        root = filter;
    }

    auto project = std::make_shared<LogicalProject>(projection_expressions_, bind_context_ptr_);
    project->set_left_node(root);
    root = project;

    if(!order_by_expressions_.empty()) {
        PlannerAssert(order_by_expressions_.size() == order_by_types_.size(), "Unknown error on order by expression");
        std::shared_ptr<LogicalNode> sort
            = std::make_shared<LogicalSort>(order_by_expressions_, order_by_types_, bind_context_ptr_);
        sort->set_left_node(root);
        root = sort;
    }

    if(limit_expression_ != nullptr) {
        std::shared_ptr<LogicalLimit> limit
            = std::make_shared<LogicalLimit>(limit_expression_, offset_expression_, bind_context_ptr_);
        limit->set_left_node(root);
        root = limit;
    }

    return root;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildFrom(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    if(table_ref) {
        switch(table_ref->type_) {
            case TableRefType::kBaseTable: return BuildBaseTable(table_ref, bind_context_ptr);
            case TableRefType::kSubquery: return BuildSubqueryTable(table_ref, bind_context_ptr);
            case TableRefType::kCrossProduct: return BuildCrossProductTable(table_ref, bind_context_ptr);
            case TableRefType::kJoin: return BuildJoinTable(table_ref, bind_context_ptr);
            case TableRefType::kDummy: return BuildDummyTable(table_ref, bind_context_ptr);
            default:
                PlannerError("Unknown table reference type.");
        }
    } else {
        // No from clause, only select constant value is OK ?
    }

    return nullptr;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildBaseTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    // std::shared_ptr<BaseTableRef> base_table_ref
    auto base_table_ref = std::static_pointer_cast<BaseTableRef>(table_ref);

    std::shared_ptr<TableFunction> table_func = Infinity::instance().catalog()->GetTableFunctionByName("seq_scan");
    std::shared_ptr<TableScanFunction> table_scan_func = std::static_pointer_cast<TableScanFunction>(table_func);

    std::shared_ptr<LogicalTableScan> table_scan_node
        = std::make_shared<LogicalTableScan>(base_table_ref->table_ptr_,
                                             bind_context_ptr,
                                             table_scan_func,
                                             base_table_ref->alias_,
                                             base_table_ref->column_names_,
                                             base_table_ref->column_types_);
    return table_scan_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildSubqueryTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    // std::shared_ptr<SubqueryTableRef> subquery_table_ref
    auto subquery_table_ref = std::static_pointer_cast<SubqueryTableRef>(table_ref);
    std::shared_ptr<LogicalNode> subquery = subquery_table_ref->subquery_node_->BuildPlan();
    return subquery;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildCrossProductTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    // std::shared_ptr<CrossProductTableRef> cross_product_table_ref
    auto cross_product_table_ref = std::static_pointer_cast<CrossProductTableRef>(table_ref);

    auto left_node = BuildFrom(cross_product_table_ref->left_table_ref_, bind_context_ptr->left_child_);
    auto right_node = BuildFrom(cross_product_table_ref->right_table_ref_, bind_context_ptr->right_child_);

    // TODO: Merge bind context ?

    std::shared_ptr<LogicalCrossProduct> logical_cross_product_node =
            std::make_shared<LogicalCrossProduct>(left_node, right_node, bind_context_ptr);
    return logical_cross_product_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildJoinTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    // std::shared_ptr<JoinTableRef> join_table_ref
    auto join_table_ref = std::static_pointer_cast<JoinTableRef>(table_ref);

    auto left_node = BuildFrom(join_table_ref->left_table_ref_, bind_context_ptr->left_child_);
    auto right_node = BuildFrom(join_table_ref->right_table_ref_, bind_context_ptr->right_child_);

    // TODO: Merge bind context ?

    std::shared_ptr<LogicalJoin> logical_join_node =
            std::make_shared<LogicalJoin>(join_table_ref->join_type_, join_table_ref->on_conditions_,
                                          left_node, right_node, bind_context_ptr);
    return logical_join_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildDummyTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
//    auto dummy_table_ref = std::static_pointer_cast<DummyTableRef>(table_ref);
    std::shared_ptr<LogicalDummyScan> dummy_scan_node = std::make_shared<LogicalDummyScan>(bind_context_ptr);
    return dummy_scan_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildFilter(std::shared_ptr<LogicalNode> root,
                            std::vector<std::shared_ptr<BaseExpression>>& conditions,
                            std::shared_ptr<BindContext>& bind_context_ptr) {
    for(auto& cond: conditions) {
        // 1. Go through all the expression to find subquery
        VisitExpression(cond,
                        [&](std::shared_ptr<BaseExpression> &expr) {
                            SubqueryUnnest::UnnestSubqueries(expr, root);
                        });
    }

    // std::shared_ptr<BaseExpression> filter_expr
    auto filter_expr = ComposeExpressionWithDelimiter(conditions, ConjunctionType::kAnd);

    // std::shared_ptr<LogicalFilter> filter
    auto filter = std::make_shared<LogicalFilter>(filter_expr, bind_context_ptr);

    return filter;
}

}
