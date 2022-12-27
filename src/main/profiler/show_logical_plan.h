//
// Created by JinHai on 2022/9/8.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"

namespace infinity {

class ShowLogicalPlan {
public:
    explicit 
    ShowLogicalPlan(SharedPtr<LogicalNode> logical_node): logical_node_(std::move(logical_node)) {}

    [[nodiscard]] String
    ToString() const;
    
private:
    SharedPtr<LogicalNode> logical_node_;
};

}



