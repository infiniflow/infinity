//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDummyScan : public LogicalNode {
public:
    explicit
    LogicalDummyScan(std::shared_ptr<BindContext>& bind_context)
        : LogicalNode(LogicalNodeType::kDummyScan, bind_context) {}

    std::string
    ToString(int64_t& space) final;
};

}
