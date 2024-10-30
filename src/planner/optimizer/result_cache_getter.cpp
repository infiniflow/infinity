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
import logical_match_tensor_scan;
import logical_match_sparse_scan;
import logical_knn_scan;
import logical_index_scan;
import cached_match;
import cached_match_scan;
import cached_index_scan;
import third_party;
import logger;
import base_table_ref;
import knn_expression;

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
        Optional<CacheOutput> cache_output;
        SharedPtr<BaseTableRef> base_table_ref;
        bool is_min_heap = false;
        switch (op->operator_type()) {
            case LogicalNodeType::kMatch: {
                auto *logical_match = static_cast<LogicalMatch *>(op.get());
                base_table_ref = logical_match->base_table_ref_;
                TxnTimeStamp query_ts = std::min(begin_ts, logical_match->base_table_ref_->max_commit_ts());
                CachedMatch cached_match(query_ts, logical_match);
                cache_output = cache_mgr->GetCache(cached_match);
                is_min_heap = true;
                break;
            }
            case LogicalNodeType::kMatchTensorScan: {
                auto *logical_match_tensor_scan = static_cast<LogicalMatchTensorScan *>(op.get());
                base_table_ref = logical_match_tensor_scan->base_table_ref_;
                TxnTimeStamp query_ts = std::min(begin_ts, logical_match_tensor_scan->base_table_ref_->max_commit_ts());
                CachedMatchTensorScan cached_match_tensor_scan(query_ts, logical_match_tensor_scan);
                cache_output = cache_mgr->GetCache(cached_match_tensor_scan);
                is_min_heap = true;
                break;
            }
            case LogicalNodeType::kMatchSparseScan: {
                auto *logical_match_sparse_scan = static_cast<LogicalMatchSparseScan *>(op.get());
                base_table_ref = logical_match_sparse_scan->base_table_ref_;
                TxnTimeStamp query_ts = std::min(begin_ts, logical_match_sparse_scan->base_table_ref_->max_commit_ts());
                CachedMatchSparseScan cached_match_sparse_scan(query_ts, logical_match_sparse_scan);
                cache_output = cache_mgr->GetCache(cached_match_sparse_scan);
                is_min_heap = true;
                break;
            }
            case LogicalNodeType::kKnnScan: {
                auto *logical_knn_scan = static_cast<LogicalKnnScan *>(op.get());
                base_table_ref = logical_knn_scan->base_table_ref_;
                TxnTimeStamp query_ts = std::min(begin_ts, logical_knn_scan->base_table_ref_->max_commit_ts());
                CachedKnnScan cached_knn_scan(query_ts, logical_knn_scan);
                cache_output = cache_mgr->GetCache(cached_knn_scan);
                is_min_heap = static_cast<const KnnExpression *>(cached_knn_scan.query_expression())->IsKnnMinHeap();
                break;
            }
            case LogicalNodeType::kIndexScan: {
                auto *logical_index_scan = static_cast<LogicalIndexScan *>(op.get());
                base_table_ref = logical_index_scan->base_table_ref_;
                TxnTimeStamp query_ts = std::min(begin_ts, logical_index_scan->base_table_ref_->max_commit_ts());
                CachedIndexScan cached_index_scan(query_ts, logical_index_scan);
                cache_output = cache_mgr->GetCache(cached_index_scan);
                break;
            }
            default: {
                break;
            }
        }
        if (!cache_output.has_value()) {
            LOG_INFO(fmt::format("No cache found for match node {}", op->node_id()));
        } else {
            LOG_INFO(fmt::format("Cache found for match node {}", op->node_id()));
            auto logical_read_cache = MakeShared<LogicalReadCache>(op->node_id(),
                                                                   op->operator_type(),
                                                                   base_table_ref,
                                                                   std::move(cache_output->cache_content_),
                                                                   std::move(cache_output->column_map_),
                                                                   is_min_heap);
            logical_read_cache->set_left_node(op->left_node());
            logical_read_cache->set_right_node(op->right_node());
            op = logical_read_cache;
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