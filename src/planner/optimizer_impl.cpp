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

module infinity_core:optimizer.impl;

import :optimizer;
import :logical_node_type;
import :logical_explain;
import :column_remapper;
import :column_pruner;
import :lazy_load;
import :index_scan_builder;
import :apply_fast_rough_filter;
import :explain_logical_plan;
import :optimizer_rule;
import :bound_delete_statement;
import :bound_update_statement;
import :logical_node;
import :result_cache_getter;

import explain_statement;
import base_statement;
import global_resource_usage;

namespace infinity {

Optimizer::Optimizer(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {
    // TODO: need an equivalent expression optimizer
    AddRule(std::make_unique<ApplyFastRoughFilter>()); // put it before SecondaryIndexScanBuilder
    AddRule(std::make_unique<IndexScanBuilder>());     // put it before ColumnPruner, necessary for filter_fulltext and index_scan
    AddRule(std::make_unique<ColumnPruner>());
    AddRule(std::make_unique<LazyLoad>());
    AddRule(std::make_unique<ColumnRemapper>());
    if (query_context_ptr->storage()->result_cache_manager()) {
        AddRule(std::make_unique<ResultCacheGetter>()); // put after column pruner, column remapper
    }

#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("Optimizer");
#endif
}

void Optimizer::AddRule(std::unique_ptr<OptimizerRule> rule) { rules_.emplace_back(std::move(rule)); }

void Optimizer::optimize(std::shared_ptr<LogicalNode> &unoptimized_plan, StatementType statement_type) {
    // Expression folding should be done in logical planner before optimizer
    // Non-select plan, the root node won't be project.
    switch (statement_type) {
        case StatementType::kSelect:
        case StatementType::kExplain:
        case StatementType::kDelete:
        case StatementType::kUpdate: {
            ; // continue;
            break;
        }
        default: {
            return;
        }
    }

    // Only work for select
    size_t rule_count = rules_.size();
    for (size_t idx = 0; idx < rule_count; ++idx) {
        const auto &rule = rules_[idx];
        rule->ApplyToPlan(query_context_ptr_, unoptimized_plan);
    }

    if (unoptimized_plan->operator_type() == LogicalNodeType::kExplain) {
        LogicalExplain *explain_node = (LogicalExplain *)(unoptimized_plan.get());
        if (explain_node->explain_type() == ExplainType::kOpt) {
            std::shared_ptr<std::vector<std::shared_ptr<std::string>>> texts_ptr = std::make_shared<std::vector<std::shared_ptr<std::string>>>();
            ExplainLogicalPlan::Explain(explain_node->left_node().get(), texts_ptr);
            explain_node->SetText(texts_ptr);
        }
    }

    return;
}

} // namespace infinity