//
// Created by JinHai on 2022/7/26.
//

#include "optimizer.h"
#include "planner/optimizer/column_remapper.h"
#include "explain_logical_plan.h"
#include "planner/node/logical_explain.h"

namespace infinity {

Optimizer::Optimizer(QueryContext* query_context_ptr) : query_context_ptr_(query_context_ptr) {
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

    if(unoptimized_plan->operator_type() == LogicalNodeType::kExplain) {
        SharedPtr<LogicalExplain> explain_node = std::static_pointer_cast<LogicalExplain>(unoptimized_plan);
        if(explain_node->explain_type() == ExplainType::kOpt) {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainLogicalPlan::Explain(explain_node->left_node().get(), texts_ptr);
            explain_node->SetText(texts_ptr);
        }
    }

    return unoptimized_plan;
}

}