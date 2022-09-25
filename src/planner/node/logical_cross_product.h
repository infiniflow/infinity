//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalCrossProduct: public LogicalNode {
public:
    explicit
    LogicalCrossProduct(int64_t node_id,
                        const std::shared_ptr<LogicalNode>& left,
                        const std::shared_ptr<LogicalNode>& right);

    std::string ToString(uint64_t space) final;
};

}
