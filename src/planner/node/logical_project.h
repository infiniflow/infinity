//
// Created by JinHai on 2022/9/26.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalProject: public LogicalNode {
public:
    LogicalProject(std::vector<std::shared_ptr<BaseExpression>> expressions, std::shared_ptr<BindContext>& bind_context)
        : LogicalNode(LogicalNodeType::kProjection, bind_context), expressions_(std::move(expressions)) {}

    std::string
    ToString(int64_t& space) final;

    std::vector<std::shared_ptr<BaseExpression>> expressions_;
};


}
