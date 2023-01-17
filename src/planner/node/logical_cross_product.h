//
// Created by JinHai on 2022/9/25.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalCrossProduct: public LogicalNode {
public:
    explicit
    LogicalCrossProduct(const SharedPtr<LogicalNode>& left,
                        const SharedPtr<LogicalNode>& right);

    String
    ToString(i64& space) final;
};

}
