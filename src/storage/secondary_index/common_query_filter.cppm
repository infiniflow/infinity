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
export module common_query_filter;
import stl;
import bitmask;
import base_expression;
import base_table_ref;
import fast_rough_filter;
import table_index_entry;
import segment_entry;
import secondary_index_scan_execute_expression;
import query_context;
import buffer_manager;

namespace infinity {

export struct CommonQueryFilter {
    SharedPtr<BaseExpression> original_filter_;
    SharedPtr<BaseTableRef> base_table_ref_;
    // input filter
    // 1. minmax and bloom filter
    bool finish_build_fast_rough_filter_ = false;
    UniquePtr<FastRoughFilterEvaluator> fast_rough_filter_evaluator_;
    // 2. filter for datablocks
    SharedPtr<BaseExpression> filter_leftover_;
    // 3. secondary index filter
    bool finish_build_secondary_index_filter_ = false;
    SharedPtr<BaseExpression> secondary_index_filter_qualified_;
    HashMap<ColumnID, TableIndexEntry *> secondary_index_column_index_map_;
    Vector<FilterExecuteElem> filter_execute_command_;

    // result
    atomic_flag finish_build_;
    Map<SegmentID, std::variant<Vector<u32>, Bitmask>> filter_result_;
    SizeT filter_result_count_ = 0;

    // task info
    Vector<SegmentID> tasks_;
    u32 total_task_num_ = 0;
    std::mutex task_mutex_;
    u32 begin_task_num_ = 0;
    atomic_u32 end_task_num_ = 0;

    CommonQueryFilter(SharedPtr<BaseExpression> original_filter, SharedPtr<BaseTableRef> base_table_ref)
        : original_filter_(std::move(original_filter)), base_table_ref_(std::move(base_table_ref)) {
        const HashMap<SegmentID, SegmentEntry *> &segment_index = base_table_ref_->block_index_->segment_index_;
        if (segment_index.empty()) {
            finish_build_.test_and_set(std::memory_order_release);
        } else {
            tasks_.reserve(segment_index.size());
            for (const auto &[segment_id, _] : segment_index) {
                tasks_.push_back(segment_id);
            }
            total_task_num_ = tasks_.size();
        }
    }

    // 1. try to finish building the filter
    // 2. return true if the filter is available for query
    // if other threads are building the filter, the filter is not available for query
    // in this case, physical operator should return early and wait for next scheduling
    bool TryFinishBuild(TxnTimeStamp begin_ts, BufferManager *buffer_mgr) {
        if (finish_build_.test(std::memory_order_acquire)) {
            return true;
        }
        while (true) {
            u32 task_id = -1;
            {
                std::scoped_lock lock(task_mutex_);
                if (begin_task_num_ == total_task_num_) {
                    break;
                }
                task_id = begin_task_num_++;
            }
            BuildFilter(task_id, begin_ts, buffer_mgr);
            if (++end_task_num_ == total_task_num_) {
                finish_build_.test_and_set(std::memory_order_release);
                break;
            }
        }
        // no more begin_task_num_ left
        // but it is possible that other threads are still building the filter
        return finish_build_.test(std::memory_order_acquire);
    }

    void TryApplyFastRoughFilterOptimizer();
    void TryApplySecondaryIndexFilterOptimizer(QueryContext *query_context);

private:
    void BuildFilter(u32 task_id, TxnTimeStamp begin_ts, BufferManager *buffer_mgr);
};

} // namespace infinity