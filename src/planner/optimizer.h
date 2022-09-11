//
// Created by JinHai on 2022/7/26.
//

#pragma once

#include "logical_node.h"
#include "optimizer_rule.h"
#include <memory>
#include <vector>

namespace infinity {

class QueryContext;

class Optimizer {
public:
    explicit Optimizer(std::shared_ptr<QueryContext> query_context_ptr)
        : query_context_ptr_(std::move(query_context_ptr)) {}

    void AddRule(std::unique_ptr<OptimizerRule> rule);

    std::shared_ptr<LogicalNode> optimize(const std::shared_ptr<LogicalNode>& unoptimized_plan);

public:
    std::shared_ptr<QueryContext> query_context_ptr_;

    std::vector<std::unique_ptr<OptimizerRule>> rules_;
};

}
