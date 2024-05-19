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

#include <string>

module physical_table_scan;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import global_block_id;
import data_block;
import table_scan_function_data;
import base_table_ref;
import block_index;

import default_values;
import infinity_exception;
import infinity_exception;
import third_party;
import logger;
import column_vector;
import infinity_exception;
import logical_type;

import block_entry;

namespace infinity {

void PhysicalTableScan::Init() {}

bool PhysicalTableScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *table_scan_operator_state = static_cast<TableScanOperatorState *>(operator_state);
    ExecuteInternal(query_context, table_scan_operator_state);
    return true;
}

SharedPtr<Vector<String>> PhysicalTableScan::GetOutputNames() const {
    if (!add_row_id_)
        return base_table_ref_->column_names_;
    auto dst = MakeShared<Vector<String>>();
    dst->reserve(base_table_ref_->column_names_->size() + 1);
    for (auto &name : *base_table_ref_->column_names_)
        dst->emplace_back(name);
    dst->emplace_back(COLUMN_NAME_ROW_ID);
    return dst;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalTableScan::GetOutputTypes() const {
    if (!add_row_id_)
        return base_table_ref_->column_types_;
    auto dst = MakeShared<Vector<SharedPtr<DataType>>>();
    dst->reserve(base_table_ref_->column_types_->size() + 1);
    for (auto &type : *base_table_ref_->column_types_)
        dst->emplace_back(type);
    dst->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return dst;
}

String PhysicalTableScan::table_alias() const { return base_table_ref_->alias_; }

u64 PhysicalTableScan::TableIndex() const { return base_table_ref_->table_index_; }

TableEntry *PhysicalTableScan::TableEntry() const { return base_table_ref_->table_entry_ptr_; }

SizeT PhysicalTableScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

SizeT PhysicalTableScan::TaskletCount() { return base_table_ref_->block_index_->BlockCount(); }

BlockIndex *PhysicalTableScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

Vector<SizeT> &PhysicalTableScan::ColumnIDs() const {
    if (!add_row_id_)
        return base_table_ref_->column_ids_;
    if (!column_ids_.empty())
        return column_ids_;
    column_ids_ = base_table_ref_->column_ids_;
    column_ids_.push_back(COLUMN_IDENTIFIER_ROW_ID);
    return column_ids_;
}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalTableScan::PlanBlockEntries(i64 parallel_count) const {
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<GlobalBlockID> global_blocks;
    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        for (const auto *block_entry : segment_info.block_map_) {
            global_blocks.emplace_back(GlobalBlockID{segment_id, block_entry->block_id()});
        }
    }
    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (SizeT task_id = 0, global_block_id = 0, residual_idx = 0; (i64)task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->emplace_back(global_blocks[global_block_id++]);
        }
        if (residual_idx < residual) {
            result[task_id]->emplace_back(global_blocks[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

void PhysicalTableScan::ExecuteInternal(QueryContext *query_context, TableScanOperatorState *table_scan_operator_state) {
    if (!table_scan_operator_state->data_block_array_.empty()) {
        UnrecoverableError("Table scan output data block array should be empty");
    }

    table_scan_operator_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
    DataBlock *output_ptr = table_scan_operator_state->data_block_array_.back().get();
    output_ptr->Init(*GetOutputTypes());

    TableScanFunctionData *table_scan_function_data_ptr = table_scan_operator_state->table_scan_function_data_.get();
    const BlockIndex *block_index = table_scan_function_data_ptr->block_index_;
    Vector<GlobalBlockID> *block_ids = table_scan_function_data_ptr->global_block_ids_.get();
    const Vector<SizeT> &column_ids = table_scan_function_data_ptr->column_ids_;
    u64 &block_ids_idx = table_scan_function_data_ptr->current_block_ids_idx_;
    if (block_ids_idx >= block_ids->size()) {
        // No data or all data is read
        table_scan_operator_state->SetComplete();
        return;
    }

    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    SizeT &read_offset = table_scan_function_data_ptr->current_read_offset_;

    {
        String out;
        for (auto &global_block_id : *block_ids) {
            out += fmt::format("({},{}) ", global_block_id.segment_id_, global_block_id.block_id_);
        }
        LOG_TRACE(fmt::format("TableScan: block_ids: {}", out));
    }

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output_ptr->available_capacity();
    while (block_ids_idx < block_ids->size()) {
        u32 segment_id = block_ids->at(block_ids_idx).segment_id_;
        u16 block_id = block_ids->at(block_ids_idx).block_id_;

        BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);
        if (read_offset == 0) {
            // new block, check FastRoughFilter
            const auto &fast_rough_filter = *current_block_entry->GetFastRoughFilter();
            if (fast_rough_filter_evaluator_ and !fast_rough_filter_evaluator_->Evaluate(begin_ts, fast_rough_filter)) {
                // skip this block
                LOG_TRACE(fmt::format("TableScan: block_ids_idx: {}, block_ids.size(): {}, skipped after apply FastRoughFilter",
                                      block_ids_idx,
                                      block_ids->size()));
                ++block_ids_idx;
                continue;
            } else {
                LOG_TRACE(fmt::format("TableScan: block_ids_idx: {}, block_ids.size(): {}, not skipped after apply FastRoughFilter",
                                      block_ids_idx,
                                      block_ids->size()));
            }
        }
        auto [row_begin, row_end] = current_block_entry->GetVisibleRange(begin_ts, read_offset);
        if (row_begin == row_end) {
            // we have read all data from current block, move to next block
            ++block_ids_idx;
            read_offset = 0;
            continue;
        }
        if (write_capacity == 0) {
            // output is full
            break;
        }
        auto write_size = std::min(write_capacity, SizeT(row_end - row_begin));

        read_offset = row_begin;
        SizeT output_column_id{0};
        for (auto column_id : column_ids) {
            if (column_id == COLUMN_IDENTIFIER_ROW_ID) {
                u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + read_offset;
                output_ptr->column_vectors[output_column_id++]->AppendWith(RowID(segment_id, segment_offset), write_size);
            } else {
                ColumnVector column_vector =
                    current_block_entry->GetColumnBlockEntry(column_id)->GetColumnVector(query_context->storage()->buffer_manager());
                output_ptr->column_vectors[output_column_id++]->AppendWith(column_vector, read_offset, write_size);
            }
        }

        // write_size = already read size = already write size
        write_capacity -= write_size;
        read_offset += write_size;
    }

    LOG_TRACE(fmt::format("TableScan: block_ids_idx: {}, block_ids.size(): {}", block_ids_idx, block_ids->size()));

    if (block_ids_idx >= block_ids->size()) {
        table_scan_operator_state->SetComplete();
    }

    output_ptr->Finalize();
}

} // namespace infinity
