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
    explicit
    LogicalFilter(SharedPtr<BaseExpression> expression)
        : LogicalNode(LogicalNodeType::kFilter), expression_(std::move(expression)) {}

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalFilter";
    }

    [[nodiscard]] inline SharedPtr<BaseExpression>&
    expression() {
        return expression_;
    }
private:
    SharedPtr<BaseExpression> expression_{};
};

}

