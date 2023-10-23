//
// Created by JinHai on 2022/7/26.
//

module;

import stl;
import logical_node_type;
import logical_explain;
import column_remapper;
import parser;
import explain_logical_plan;

module optimizer;

namespace infinity {

Optimizer::Optimizer(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) { AddRule(MakeShared<ColumnRemapper>()); }

void Optimizer::AddRule(SharedPtr<OptimizerRule> rule) { rules_.emplace_back(Move(rule)); }

SharedPtr<LogicalNode> Optimizer::optimize(const SharedPtr<LogicalNode> &unoptimized_plan) {
    SizeT rule_count = rules_.size();
    for(SizeT idx = 0; idx < rule_count; ++ idx) {
        const auto& rule = rules_[idx];
        rule->ApplyToPlan(query_context_ptr_, unoptimized_plan);
    }

    if (unoptimized_plan->operator_type() == LogicalNodeType::kExplain) {
        LogicalExplain* explain_node = (LogicalExplain*)(unoptimized_plan.get());
        if (explain_node->explain_type() == ExplainType::kOpt) {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainLogicalPlan::Explain(explain_node->left_node().get(), texts_ptr);
            explain_node->SetText(texts_ptr);
        }
    }

    return unoptimized_plan;
}

} // namespace infinity