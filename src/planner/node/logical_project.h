//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalProject: public LogicalNode {
public:
    LogicalProject(Vector<SharedPtr<BaseExpression>> expressions)
        : LogicalNode(LogicalNodeType::kProjection), expressions_(std::move(expressions)) {}

    String
    ToString(i64& space) final;

    Vector<SharedPtr<BaseExpression>> expressions_{};
};


}
