//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"
#include "common/types.h"

namespace infinity {


class LogicalSort : public LogicalNode {
public:
    LogicalSort(Vector<SharedPtr<BaseExpression>> expressions,
                Vector<OrderByType> order_by_types)
    : LogicalNode(LogicalNodeType::kSort),
    expressions_(std::move(expressions)),
    order_by_types_(std::move(order_by_types)) {}

    String
    ToString(i64& space) final;

private:
    Vector<SharedPtr<BaseExpression>> expressions_{};
    Vector<OrderByType> order_by_types_{};
};

}