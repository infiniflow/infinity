//
// Created by jinhai on 23-1-17.
//

#pragma once

#include "planner/logical_node.h"
#include "common/types/data_type.h"

namespace infinity {

struct BoundStatement {
public:
    SharedPtr<LogicalNode> logical_plan_{};
    SharedPtr<Vector<String>> names_ptr_{};
    SharedPtr<Vector<DataType>> types_ptr_{};

    virtual SharedPtr<LogicalNode>
    BuildPlan() = 0;
};

}

