//
// Created by JinHai on 2022/7/23.
//

#include "logical_node.h"

namespace infinity {

LogicalNode::LogicalNode(int64_t node_id, LogicalNodeType node_type)
        : node_id_(node_id), operator_type_(node_type) {}

void
LogicalNode::AddOutputNode(const std::shared_ptr<LogicalNode> &output) {

}

void
LogicalNode::RemoveOutputNode(const std::shared_ptr<LogicalNode> &output) {

}

void
LogicalNode::ClearOutputs() {

}

}