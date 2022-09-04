//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include "logical_node.h"
#include "optimizer_rule.h"
#include <memory>
#include <vector>

namespace infinity {

class Optimizer {
public:
    Optimizer() = default;
    void AddRule(std::unique_ptr<OptimizerRule> rule);

    std::shared_ptr<LogicalNode> optimize(const std::shared_ptr<LogicalNode>& unoptimized_plan);

public:
    std::vector<std::unique_ptr<OptimizerRule>> rules_;
};

}
