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
    explicit Optimizer(QueryContext *query_context_ptr);

    void AddRule(SharedPtr<OptimizerRule> rule);

    SharedPtr<LogicalNode> optimize(const SharedPtr<LogicalNode> &unoptimized_plan);

public:
    QueryContext *query_context_ptr_{};

    Vector<SharedPtr<OptimizerRule>> rules_{};
};

} // namespace infinity
