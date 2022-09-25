//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalCrossProduct: public LogicalNode {
public:
    explicit
    LogicalCrossProduct(const std::shared_ptr<LogicalNode>& left,
                        const std::shared_ptr<LogicalNode>& right,
                        std::shared_ptr<BindContext>& bind_context);

    std::string ToString(uint64_t space) final;
};

}
