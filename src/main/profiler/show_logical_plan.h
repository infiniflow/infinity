//
// Created by JinHai on 2022/9/8.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"

namespace infinity {

class ShowLogicalPlan {
public:
    explicit ShowLogicalPlan(std::shared_ptr<LogicalNode> logical_node): logical_node_(std::move(logical_node)) {}

    [[nodiscard]] std::string ToString() const;
private:
    std::shared_ptr<LogicalNode> logical_node_;
};

}



