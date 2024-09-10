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
#include <bit>
#include <cassert>
#include <vector>
module common_query_filter;
import stl;
import roaring_bitmap;
import base_expression;
import base_table_ref;
import block_index;
import segment_entry;
import fast_rough_filter;
import table_index_entry;
import secondary_index_scan_execute_expression;
import filter_value_type_classification;
import physical_index_scan;
import filter_expression_push_down;
import data_block;
import buffer_manager;
import expression_evaluator;
import block_entry;
import default_values;
import internal_types;
import column_vector;
import segment_iter;
import vector_buffer;
import data_type;
import logical_type;
import expression_state;
import infinity_exception;
import third_party;
import logger;

namespace infinity {

void ReadDataBlock(DataBlock *output,
                   BufferManager *buffer_mgr,
                   const SizeT row_count,
                   const BlockEntry *current_block_entry,
                   const Vector<SizeT> &column_ids) {
    auto block_id = current_block_entry->block_id();
    auto segment_id = current_block_entry->segment_id();
    for (SizeT i = 0; i < column_ids.size(); ++i) {
        SizeT column_id = column_ids[i];
        if (column_id == COLUMN_IDENTIFIER_ROW_ID) {
            u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY;
            output->column_vectors[i]->AppendWith(RowID(segment_id, segment_offset), row_count);
        } else {
            ColumnVector column_vector = current_block_entry->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr);
            output->column_vectors[i]->AppendWith(column_vector, 0, row_count);
        }
    }
    output->Finalize();
}

void MergeFalseIntoBitmask(const VectorBuffer *input_bool_column_buffer,
                           const SharedPtr<Bitmask> &input_null_mask,
                           const SizeT count,
                           Bitmask &bitmask,
                           const SizeT bitmask_offset) {
    input_null_mask->RoaringBitmapApplyFunc([&](const u32 row_offset) -> bool {
        if (row_offset >= count) {
            return false;
        }
        if (!(input_bool_column_buffer->GetCompactBit(row_offset))) {
            bitmask.SetFalse(row_offset + bitmask_offset);
        }
        return row_offset + 1 < count;
    });
}

CommonQueryFilter::CommonQueryFilter(SharedPtr<BaseExpression> original_filter, SharedPtr<BaseTableRef> base_table_ref, TxnTimeStamp begin_ts)
    : begin_ts_(begin_ts), original_filter_(std::move(original_filter)), base_table_ref_(std::move(base_table_ref)) {
    const auto &segment_index = base_table_ref_->block_index_->segment_block_index_;
    if (segment_index.empty()) {
        finish_build_.test_and_set(std::memory_order_release);
    } else {
        tasks_.reserve(segment_index.size());
        for (const auto &[segment_id, _] : segment_index) {
            tasks_.push_back(segment_id);
        }
        total_task_num_ = tasks_.size();
    }
    always_true_ = original_filter_ == nullptr && !base_table_ref_->table_entry_ptr_->CheckAnyDelete(begin_ts_);
    if (always_true_)
        finish_build_.test_and_set(std::memory_order_release);
}

void CommonQueryFilter::BuildFilter(u32 task_id, Txn *txn) {
    auto *buffer_mgr = txn->buffer_mgr();
    TxnTimeStamp begin_ts = txn->BeginTS();
    const auto &segment_index = base_table_ref_->block_index_->segment_block_index_;
    const SegmentID segment_id = tasks_[task_id];
    const SegmentEntry *segment_entry = segment_index.at(segment_id).segment_entry_;
    if (!fast_rough_filter_evaluator_->Evaluate(begin_ts, *segment_entry->GetFastRoughFilter())) {
        // skip this segment
        return;
    }
    const SizeT segment_row_count = segment_index.at(segment_id).segment_offset_;
    Bitmask result_elem = SolveSecondaryIndexFilter(filter_execute_command_, secondary_index_column_index_map_, segment_id, segment_row_count, txn);
    if (result_elem.CountTrue() == 0) {
        // empty result
        return;
    }
    if (result_elem.count() != segment_row_count) {
        UnrecoverableError(fmt::format("Segment_row_count mismatch: In segment {}: segment_row_count: {}, result_elem.count(): {}",
                                       segment_id,
                                       segment_row_count,
                                       result_elem.count()));
    }
    if (filter_leftover_) {
        SizeT segment_row_count_read = 0;
        auto filter_state = ExpressionState::CreateState(filter_leftover_);
        auto db_for_filter_p = MakeUnique<DataBlock>();
        auto db_for_filter = db_for_filter_p.get();
        db_for_filter->Init(*(base_table_ref_->column_types_));
        auto bool_column = ColumnVector::Make(MakeShared<infinity::DataType>(LogicalType::kBoolean));
        // filter and build bitmask, if filter_expression_ != nullptr
        ExpressionEvaluator expr_evaluator;
        auto block_entry_iter = BlockEntryIter(segment_entry);
        for (auto *block_entry = block_entry_iter.Next(); block_entry != nullptr and segment_row_count_read < segment_row_count;
             block_entry = block_entry_iter.Next()) {
            const auto block_row_count = block_entry->row_count();
            const auto row_count = std::min<SizeT>(segment_row_count - segment_row_count_read, block_row_count);
            db_for_filter->Reset(row_count);
            ReadDataBlock(db_for_filter, buffer_mgr, row_count, block_entry, base_table_ref_->column_ids_);
            bool_column->Initialize(ColumnVectorType::kCompactBit, row_count);
            expr_evaluator.Init(db_for_filter);
            expr_evaluator.Execute(filter_leftover_, filter_state, bool_column);
            const VectorBuffer *bool_column_buffer = bool_column->buffer_.get();
            SharedPtr<Bitmask> &null_mask = bool_column->nulls_ptr_;
            MergeFalseIntoBitmask(bool_column_buffer, null_mask, row_count, result_elem, segment_row_count_read);
            segment_row_count_read += row_count;
            bool_column->Reset();
        }
        if (segment_row_count_read < segment_row_count) {
            UnrecoverableError(fmt::format("Segment_row_count mismatch: In segment {}: segment_row_count_read: {}, segment_row_count: {}",
                                           segment_id,
                                           segment_row_count_read,
                                           segment_row_count));
        }
    }
    // Remove deleted rows from the result
    segment_entry->CheckRowsVisible(result_elem, begin_ts);
    result_elem.RunOptimize();
    if (const auto result_count = result_elem.CountTrue(); result_count) {
        std::lock_guard lock(result_mutex_);
        filter_result_count_ += result_count;
        filter_result_.emplace(segment_id, std::move(result_elem));
    }
}

void CommonQueryFilter::TryApplyFastRoughFilterOptimizer() {
    if (finish_build_fast_rough_filter_) {
        return;
    }
    finish_build_fast_rough_filter_ = true;
    fast_rough_filter_evaluator_ = FilterExpressionPushDown::PushDownToFastRoughFilter(original_filter_);
}

void CommonQueryFilter::TryApplySecondaryIndexFilterOptimizer(QueryContext *query_context) {
    if (finish_build_secondary_index_filter_) {
        return;
    }
    finish_build_secondary_index_filter_ = true;
    if (!original_filter_) {
        return;
    }
    IndexScanFilterExpressionPushDownResult index_scan_solve_result =
        FilterExpressionPushDown::PushDownToIndexScan(query_context, *base_table_ref_, original_filter_);
    auto &column_index_map = index_scan_solve_result.column_index_map_;
    auto &v_qualified = index_scan_solve_result.index_filter_qualified_;
    auto &s_leftover = index_scan_solve_result.extra_leftover_filter_;
    auto &filter_execute_command = index_scan_solve_result.filter_execute_command_;
    filter_leftover_ = std::move(s_leftover);
    secondary_index_filter_qualified_ = std::move(v_qualified);
    secondary_index_column_index_map_ = std::move(column_index_map);
    filter_execute_command_ = std::move(filter_execute_command);
}

bool CommonQueryFilter::PassFilter(RowID doc_id) {
    if (always_true_) [[unlikely]]
        return true;
    bool finish_build = finish_build_.test();
    assert(finish_build);
    if (!finish_build)
        return false;
    if (doc_id.segment_id_ != current_segment_id_) [[unlikely]] {
        const auto it = filter_result_.find(doc_id.segment_id_);
        if (it == filter_result_.end()) [[unlikely]] {
            current_segment_id_ = INVALID_SEGMENT_ID;
            return false;
        }
        current_segment_id_ = doc_id.segment_id_;
        doc_id_bitmask_ = &(it->second);
    }
    return doc_id_bitmask_->IsTrue(doc_id.segment_offset_);
}

} // namespace infinity