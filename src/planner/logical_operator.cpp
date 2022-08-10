//
// Created by JinHai on 2022/7/23.
//

#include "logical_operator.h"

namespace infinity {

LogicalOperator::LogicalOperator(LogicalOperatorType node_type)
        : operator_type_(node_type) {}

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