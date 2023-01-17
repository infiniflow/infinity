//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDummyScan : public LogicalNode {
public:
    explicit
    LogicalDummyScan()
        : LogicalNode(LogicalNodeType::kDummyScan) {}

    String
    ToString(i64& space) final;
};

}
