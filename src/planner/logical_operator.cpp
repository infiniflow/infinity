//
// Created by JinHai on 2022/7/23.
//

#include "logical_operator.h"

namespace infinity {

uint64_t LogicalOperator::node_id_count_{0};

LogicalOperator::LogicalOperator(LogicalOperatorType node_type, uint64_t node_id)
        : node_type_(node_type), node_id_(node_id) {}

void
LogicalOperator::AddOutputNode(const std::shared_ptr<LogicalOperator> &output) {

}

void
LogicalOperator::RemoveOutputNode(const std::shared_ptr<LogicalOperator> &output) {

}

void
LogicalOperator::ClearOutputs() {

}

}