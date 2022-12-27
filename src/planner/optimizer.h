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
    explicit
    Optimizer(SharedPtr<QueryContext> query_context_ptr)
        : query_context_ptr_(std::move(query_context_ptr)) {}

    void
    AddRule(UniquePtr<OptimizerRule> rule);

    SharedPtr<LogicalNode>
    optimize(const SharedPtr<LogicalNode>& unoptimized_plan);

public:
    SharedPtr<QueryContext> query_context_ptr_;

    Vector<UniquePtr<OptimizerRule>> rules_;
};

}
