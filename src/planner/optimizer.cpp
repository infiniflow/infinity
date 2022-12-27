//
// Created by JinHai on 2022/7/26.
//

#include "optimizer.h"

namespace infinity {

void
Optimizer::AddRule(UniquePtr<OptimizerRule> rule) {
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