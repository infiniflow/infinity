//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalProject: public LogicalNode {
public:
    LogicalProject(Vector<SharedPtr<BaseExpression>> expressions,
                   u64 projection_index)
        : LogicalNode(LogicalNodeType::kProjection),
        expressions_(std::move(expressions)),
        table_index_(projection_index)
        {}

    String
    ToString(i64& space) final;

    Vector<SharedPtr<BaseExpression>> expressions_{};

    u64 table_index_{};
};


}
