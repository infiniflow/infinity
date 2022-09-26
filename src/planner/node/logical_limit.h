//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalLimit: public LogicalNode {
public:
    LogicalLimit(std::shared_ptr<BaseExpression> limit_expression,
                 std::shared_ptr<BaseExpression> offset_expression,
                 std::shared_ptr<BindContext>& bind_context)
            : LogicalNode(LogicalNodeType::kLimit, bind_context),
            limit_expression_(std::move(limit_expression)),
            offset_expression_(std::move(offset_expression)) {}

    std::string
    ToString(int64_t& space) final;

private:
    std::shared_ptr<BaseExpression> limit_expression_;
    std::shared_ptr<BaseExpression> offset_expression_;
};


}
