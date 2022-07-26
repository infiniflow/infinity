//
// Created by JinHai on 2022/7/26.
//

#include "optimizer.h"

namespace infinity {

void
Optimizer::AddRule(std::unique_ptr<OptimizerRule> rule) {
    rules_.push_back(std::move(rule));
}

std::shared_ptr<LogicalOperator>
Optimizer::optimize(const std::shared_ptr<LogicalOperator>& unoptimized_plan) {
    for(const auto& rule : rules_) {
        rule->ApplyToPlan(unoptimized_plan);
    }
    return unoptimized_plan;
}

}