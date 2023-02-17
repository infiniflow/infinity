//
// Created by JinHai on 2022/7/26.
//

#include "optimizer.h"
#include "planner/optimizer/column_remapper.h"

namespace infinity {

Optimizer::Optimizer(SharedPtr<QueryContext> query_context_ptr) : query_context_ptr_(std::move(query_context_ptr)) {
    AddRule(MakeShared<ColumnRemapper>());
}

void
Optimizer::AddRule(SharedPtr<OptimizerRule> rule) {
    rules_.emplace_back(std::move(rule));
}

SharedPtr<LogicalNode>
Optimizer::optimize(const SharedPtr<LogicalNode>& unoptimized_plan) {
    for(const auto& rule : rules_) {
        rule->ApplyToPlan(query_context_ptr_, unoptimized_plan);
    }
    return unoptimized_plan;
}

}