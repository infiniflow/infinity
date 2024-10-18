//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

#include <vector>

module result_cache_getter;

import logical_match;
import result_cache_manager;
import logical_node_type;
import logical_read_cache;
import logical_fusion;
import cached_match;

namespace infinity {

void ResultCacheGetter::ApplyToPlan(QueryContext *query_context_ptr, SharedPtr<LogicalNode> &logical_plan) {
    ResultCacheManager *cache_mgr = query_context_ptr->storage()->result_cache_manager();
    if (cache_mgr == nullptr) {
        return;
    }
    TxnTimeStamp begin_ts = query_context_ptr->GetTxn()->BeginTS();
    std::function<void(SharedPtr<LogicalNode> &)> visit_node = [&](SharedPtr<LogicalNode> &op) {
        if (!op) {
            return;
        }
        switch (op->operator_type()) {
            case LogicalNodeType::kMatch: {
                auto *logical_match = static_cast<LogicalMatch *>(op.get());
                TxnTimeStamp query_ts = std::min(begin_ts, logical_match->base_table_ref_->max_commit_ts());
                CachedMatch cached_match(query_ts, logical_match);
                Optional<CacheOutput> cache_output = cache_mgr->GetCache(cached_match);
                if (!cache_output.has_value()) {
                    break;
                }
                auto logical_read_cache = MakeShared<LogicalReadCache>(logical_match->node_id(),
                                                                       LogicalNodeType::kMatch,
                                                                       logical_match->base_table_ref_,
                                                                       std::move(cache_output->cache_content_),
                                                                       std::move(cache_output->column_map_));
                logical_read_cache->set_left_node(op->left_node());
                logical_read_cache->set_right_node(op->right_node());
                op = logical_read_cache;
                break;
            }
            default: {
                //
            }
        }
        // visit children after handling current node
        visit_node(op->left_node());
        visit_node(op->right_node());
        if (op->operator_type() == LogicalNodeType::kFusion) {
            for (auto &fusion = static_cast<LogicalFusion &>(*op); auto &child : fusion.other_children_) {
                visit_node(child);
            }
        }
    };
    visit_node(logical_plan);
}

} // namespace infinity