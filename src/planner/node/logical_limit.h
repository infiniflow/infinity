//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalLimit: public LogicalNode {
public:
    explicit
    LogicalLimit(SharedPtr<BaseExpression> limit_expression,
                 SharedPtr<BaseExpression> offset_expression)
            : LogicalNode(LogicalNodeType::kLimit),
            limit_expression_(std::move(limit_expression)),
            offset_expression_(std::move(offset_expression)) {}

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalLimit";
    }

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> offset_expression_{};
};


}
