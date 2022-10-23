//
// Created by JinHai on 2022/7/23.
//

#include "logical_node.h"

namespace infinity {

LogicalNode::LogicalNode(LogicalNodeType node_type, std::shared_ptr<BindContext>& bind_context_ptr)
        : operator_type_(node_type), bind_context_ptr_(bind_context_ptr) {
    node_id_ = bind_context_ptr->GetNewLogicalNodeId();
}

//void
//LogicalNode::AddOutputNode(const std::shared_ptr<LogicalNode> &output) {
//
//}
//
//void
//LogicalNode::RemoveOutputNode(const std::shared_ptr<LogicalNode> &output) {
//
//}
//
//void
//LogicalNode::ClearOutputs() {
//
//}

}