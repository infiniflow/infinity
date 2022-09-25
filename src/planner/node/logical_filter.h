//
// Created by JinHai on 2022/8/9.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"
#include "expression/base_expression.h"

namespace infinity {

class LogicalFilter : public LogicalNode {
public:
    LogicalFilter(int64_t node_id, std::shared_ptr<BaseExpression> expression)
        : LogicalNode(node_id, LogicalNodeType::kFilter), expression_(std::move(expression)) {}

    std::string ToString(uint64_t space) final;

private:
    std::shared_ptr<BaseExpression> expression_;
};

}

