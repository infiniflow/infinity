//
// Created by JinHai on 2022/8/9.
//

#pragma once

#include <utility>

#include "planner/logical_operator.h"
#include "expression/base_expression.h"

namespace infinity {

class LogicalFilter : public LogicalOperator {
public:
    LogicalFilter(uint64_t node_id, std::shared_ptr<BaseExpression> expression)
        : LogicalOperator(LogicalOperatorType::kFilter, node_id), expression_(std::move(expression)) {}

    std::string ToString(uint64_t space) final;

private:
    std::shared_ptr<BaseExpression> expression_;
};

}

