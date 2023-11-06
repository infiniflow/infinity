// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import logical_node_type;
import logical_explain;
import column_remapper;
import column_pruner;
import parser;
import explain_logical_plan;

module optimizer;

namespace infinity {

Optimizer::Optimizer(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {
    AddRule(MakeShared<ColumnPruner>());
    AddRule(MakeShared<ColumnRemapper>());
}

void Optimizer::AddRule(SharedPtr<OptimizerRule> rule) { rules_.emplace_back(Move(rule)); }

SharedPtr<LogicalNode> Optimizer::optimize(const SharedPtr<LogicalNode> &unoptimized_plan) {
    SizeT rule_count = rules_.size();
    for (SizeT idx = 0; idx < rule_count; ++idx) {
        const auto &rule = rules_[idx];
        rule->ApplyToPlan(query_context_ptr_, unoptimized_plan);
    }

    if (unoptimized_plan->operator_type() == LogicalNodeType::kExplain) {
        LogicalExplain *explain_node = (LogicalExplain *)(unoptimized_plan.get());
        if (explain_node->explain_type() == ExplainType::kOpt) {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainLogicalPlan::Explain(explain_node->left_node().get(), texts_ptr);
            explain_node->SetText(texts_ptr);
        }
    }

    return unoptimized_plan;
}

} // namespace infinity