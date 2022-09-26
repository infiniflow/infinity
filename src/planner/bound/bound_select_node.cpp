//
// Created by JinHai on 2022/9/12.
//

#include "bound_select_node.h"
#include "common/utility/infinity_assert.h"
#include "planner/node/logical_cross_product.h"
#include "planner/node/logical_join.h"
#include "subquery_unnest.h"

namespace infinity {
std::shared_ptr<LogicalNode>
BoundSelectNode::BuildPlan() {

    std::shared_ptr<LogicalNode> root = BuildFrom(table_ref_ptr_, bind_context_ptr_);
    if(!where_conditions_.empty()) {
        std::shared_ptr<LogicalNode> filter = BuildFilter(root, where_conditions_, bind_context_ptr_);
        filter->set_left_node(root);
        root = filter;
    }

    return nullptr;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildFrom(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    if(table_ref) {
        switch(table_ref->type_) {
            case TableRefType::kBaseTable: return BuildBaseTable(table_ref, bind_context_ptr);
            case TableRefType::kSubquery: return BuildSubqueryTable(table_ref, bind_context_ptr);
            case TableRefType::kCrossProduct: return BuildCrossProductTable(table_ref, bind_context_ptr);
            case TableRefType::kJoin: return BuildJoinTable(table_ref, bind_context_ptr);
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

    std::shared_ptr<LogicalTableScan> table_scan_node
        = std::make_shared<LogicalTableScan>(base_table_ref->table_ptr_, bind_context_ptr);
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

    auto left_node = BuildFrom(cross_product_table_ref->left_table_ref_, bind_context_ptr->children_[0]);
    auto right_node = BuildFrom(cross_product_table_ref->right_table_ref_, bind_context_ptr->children_[1]);

    // TODO: Merge bind context ?

    std::shared_ptr<LogicalCrossProduct> logical_cross_product_node =
            std::make_shared<LogicalCrossProduct>(left_node, right_node, bind_context_ptr);
    return logical_cross_product_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildJoinTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    // std::shared_ptr<JoinTableRef> join_table_ref
    auto join_table_ref = std::static_pointer_cast<JoinTableRef>(table_ref);

    auto left_node = BuildFrom(join_table_ref->left_table_ref_, bind_context_ptr->children_[0]);
    auto right_node = BuildFrom(join_table_ref->right_table_ref_, bind_context_ptr->children_[1]);

    // TODO: Merge bind context ?

    std::shared_ptr<LogicalJoin> logical_join_node =
            std::make_shared<LogicalJoin>(join_table_ref->join_type_, join_table_ref->on_conditions_,
                                          left_node, right_node, bind_context_ptr);
    return logical_join_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildFilter(std::shared_ptr<LogicalNode> root,
                            std::vector<std::shared_ptr<BaseExpression>>& conditions,
                            std::shared_ptr<BindContext>& bind_context_ptr) {
    for(auto& cond: conditions) {
        SubqueryUnnest::UnnestSubqueries(cond, root);
    }
    return nullptr;
}

}
