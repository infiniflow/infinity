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

#include <algorithm>
#include <vector>

module physical_index_scan;

import query_context;
import operator_state;
import default_values;
import buffer_handle;
import infinity_exception;
import logger;
import third_party;
import txn;
import data_block;
import logical_type;
import table_index_entry;
import segment_index_entry;
import chunk_index_entry;
import secondary_index_in_mem;
import segment_entry;
import fast_rough_filter;
import roaring_bitmap;
import filter_value_type_classification;
import physical_scan_base;
import result_cache_manager;

namespace infinity {

PhysicalIndexScan::PhysicalIndexScan(const u64 id,
                                     SharedPtr<BaseTableRef> base_table_ref,
                                     SharedPtr<BaseExpression> index_filter,
                                     UniquePtr<IndexFilterEvaluator> &&index_filter_evaluator,
                                     UniquePtr<FastRoughFilterEvaluator> &&fast_rough_filter_evaluator,
                                     SharedPtr<Vector<LoadMeta>> load_metas,
                                     SharedPtr<Vector<String>> output_names,
                                     SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                     const bool add_row_id,
                                     bool cache_result)
    : PhysicalScanBase(id, PhysicalOperatorType::kIndexScan, nullptr, nullptr, 0, std::move(base_table_ref), std::move(load_metas), cache_result),
      index_filter_(std::move(index_filter)), index_filter_evaluator_(std::move(index_filter_evaluator)),
      fast_rough_filter_evaluator_(std::move(fast_rough_filter_evaluator)), output_names_(std::move(output_names)),
      output_types_(std::move(output_types)), add_row_id_(add_row_id) {
    // check last output column
    if (output_types_->size() != output_names_->size()) {
        UnrecoverableError("PhysicalIndexScan::PhysicalIndexScan(): output_names_ mismatch with output_types_");
    }
    if (!(add_row_id_ ? (output_types_->size() == 1 && output_types_->back()->type() == LogicalType::kRowID) : (output_types_->size() == 0))) {
        UnrecoverableError("PhysicalIndexScan::PhysicalIndexScan(): add_row_id_ mismatch with output_types_");
    }
}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalIndexScan::PlanBlockEntries(i64) const {
    UnrecoverableError("PhysicalIndexScan::PlanBlockEntries(): should not be called.");
    return {};
}

Vector<UniquePtr<Vector<SegmentID>>> PhysicalIndexScan::PlanSegments(u32 parallel_count) const {
    const u32 total_segment_num = base_table_ref_->block_index_->SegmentCount();
    const u32 segment_num_per_tasklet = total_segment_num / parallel_count;
    const u32 segment_num_remainder = total_segment_num % parallel_count;
    SegmentID next_segment_id = 0;
    Vector<UniquePtr<Vector<SegmentID>>> result;
    result.reserve(parallel_count);
    for (u32 i = 0; i < parallel_count; ++i) {
        auto segment_ids = MakeUnique<Vector<SegmentID>>();
        u32 segment_num = segment_num_per_tasklet + (i < segment_num_remainder ? 1 : 0);
        segment_ids->reserve(segment_num);
        for (u32 j = 0; j < segment_num; ++j) {
            segment_ids->emplace_back(next_segment_id++);
        }
        result.emplace_back(std::move(segment_ids));
    }
    if (next_segment_id != total_segment_num) {
        String error_message = "PhysicalIndexScan::PlanSegments(): segment number error.";
        UnrecoverableError(error_message);
    }
    return result;
}

void PhysicalIndexScan::Init() {
    // check add_row_id_
    if (!add_row_id_) {
        String error_message = "ExecuteInternal(): add_row_id_ should be true.";
        UnrecoverableError(error_message);
    }
}

bool PhysicalIndexScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto index_scan_operator_state = static_cast<IndexScanOperatorState *>(operator_state);
    // solve one segment in one call
    ExecuteInternal(query_context, index_scan_operator_state);
    return true;
}

void PhysicalIndexScan::ExecuteInternal(QueryContext *query_context, IndexScanOperatorState *index_scan_operator_state) const {
    Txn *txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto &output_data_blocks = index_scan_operator_state->data_block_array_;
    auto &segment_ids = *(index_scan_operator_state->segment_ids_);
    auto &next_idx = index_scan_operator_state->next_idx_;
    if (!output_data_blocks.empty()) {
        UnrecoverableError("Index scan output data block array should be empty");
    }
    // check before execute
    if (next_idx >= segment_ids.size()) {
        // Already finished
        index_scan_operator_state->SetComplete();
        return;
    }
    // get the segment id to solve
    auto segment_id = segment_ids[next_idx];

    SegmentEntry *segment_entry = nullptr;
    SegmentOffset segment_row_count = 0; // count of rows in segment, include deleted rows
    const auto &segment_block_index_ = base_table_ref_->block_index_->segment_block_index_;
    if (auto iter = segment_block_index_.find(segment_id); iter == segment_block_index_.end()) {
        UnrecoverableError(fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id));
    } else {
        segment_entry = iter->second.segment_entry_;
        segment_row_count = iter->second.segment_offset_;
    }
    // output result
    auto OutputBitmaskResult = [&](const Bitmask &result) {
        Vector<SharedPtr<DataType>> output_types;
        output_types.emplace_back(MakeShared<DataType>(LogicalType::kRowID));
        constexpr u32 block_capacity = DEFAULT_BLOCK_CAPACITY;
        const u32 selected_row_num = result.CountTrue();
        // 1. prepare first output_data_block
        auto append_data_block = [&] {
            auto data_block = DataBlock::MakeUniquePtr();
            data_block->Init(output_types);
            output_data_blocks.emplace_back(std::move(data_block));
        };
        // some operator expect at least one input block
        append_data_block();
        // 2. output
        // delete_filter: return false if the row is deleted
        u32 output_rows = 0;
        {
            u32 output_block_row_id = 0;
            DataBlock *output_block_ptr = output_data_blocks.back().get();
            result.RoaringBitmapApplyFunc([&](const u32 segment_offset) -> bool {
                if (segment_offset >= segment_row_count) [[unlikely]] {
                    UnrecoverableError("IndexScan: Output segment_offset out of range.");
                }
                if (output_block_row_id == block_capacity) {
                    output_block_ptr->Finalize();
                    append_data_block();
                    output_block_ptr = output_data_blocks.back().get();
                    output_block_row_id = 0;
                }
                RowID row_id(segment_id, segment_offset);
                output_block_ptr->AppendValueByPtr(0, reinterpret_cast<ptr_t>(&row_id));
                ++output_block_row_id;
                ++output_rows;
                return true;
            });
            output_block_ptr->Finalize();
            if (output_rows != selected_row_num) {
                UnrecoverableError("IndexScan: Output row num error.");
            }
        }
        LOG_TRACE(fmt::format("IndexScan: Output rows: {}", output_rows));
        // update next_idx
        // check if jobs are all done
        if (++next_idx >= segment_ids.size()) {
            // Finished
            index_scan_operator_state->SetComplete();
        }

        ResultCacheManager *cache_mgr = query_context->storage()->result_cache_manager();
        if (cache_result_ && cache_mgr != nullptr) {
            AddCache(query_context, cache_mgr, output_data_blocks);
        }
    };
    // check FastRoughFilter
    const auto &fast_rough_filter = *segment_entry->GetFastRoughFilter();
    if (fast_rough_filter_evaluator_ and !fast_rough_filter_evaluator_->Evaluate(begin_ts, fast_rough_filter)) {
        // skip this segment
        LOG_TRACE(fmt::format("IndexScan: job number: {}, segment_ids.size(): {}, skipped after FastRoughFilter", next_idx, segment_ids.size()));
        Bitmask result_empty(segment_row_count);
        result_empty.SetAllFalse();
        OutputBitmaskResult(result_empty);
        return;
    }
    LOG_TRACE(fmt::format("IndexScan: job number: {}, segment_ids.size(): {}, not skipped after FastRoughFilter", next_idx, segment_ids.size()));

    Bitmask result_elem = index_filter_evaluator_->Evaluate(segment_id, segment_row_count, txn);
    if (result_elem.CountTrue() > 0) {
        // Remove deleted rows from the result
        segment_entry->CheckRowsVisible(result_elem, begin_ts);
    }
    // output
    OutputBitmaskResult(result_elem);
    LOG_TRACE(fmt::format("IndexScan: job number: {}, segment_ids.size(): {}, finished", next_idx, segment_ids.size()));
}

} // namespace infinity
