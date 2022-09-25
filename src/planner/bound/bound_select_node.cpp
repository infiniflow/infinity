//
// Created by JinHai on 2022/9/12.
//

#include "bound_select_node.h"
#include "common/utility/infinity_assert.h"
#include "planner/node/logical_cross_product.h"

namespace infinity {
std::shared_ptr<LogicalNode>
BoundSelectNode::BuildPlan() {

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
    return base_table_ref->logical_table_scan_;
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

    int64_t logical_node_id = bind_context_ptr->GetNewLogicalNodeId();

    auto left_node = BuildFrom(cross_product_table_ref->left_table_ref_, bind_context_ptr->children_[0]);
    auto right_node = BuildFrom(cross_product_table_ref->right_table_ref_, bind_context_ptr->children_[1]);

    std::shared_ptr<LogicalCrossProduct> logical_cross_product_node =
            std::make_shared<LogicalCrossProduct>(logical_node_id, left_node, right_node);
    return logical_cross_product_node;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildJoinTable(std::shared_ptr<TableRef>& table_ref, std::shared_ptr<BindContext>& bind_context_ptr) {
    // std::shared_ptr<JoinTableRef> join_table_ref
    auto join_table_ref = std::static_pointer_cast<JoinTableRef>(table_ref);
    return nullptr;
}

std::shared_ptr<LogicalNode>
BoundSelectNode::BuildFilter() {
    return nullptr;
}

}
