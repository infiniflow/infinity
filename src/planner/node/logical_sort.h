//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"
#include "common/types.h"

namespace infinity {


class LogicalSort : public LogicalNode {
public:
    LogicalSort(std::vector<std::shared_ptr<BaseExpression>> expressions,
                std::vector<OrderByType> order_by_types,
                std::shared_ptr<BindContext>& bind_context)
    : LogicalNode(LogicalNodeType::kSort, bind_context),
    expressions_(std::move(expressions)),
    order_by_types_(std::move(order_by_types)) {}

    std::string
    ToString(int64_t& space) final;

private:
    std::vector<std::shared_ptr<BaseExpression>> expressions_;
    std::vector<OrderByType> order_by_types_;
};

}