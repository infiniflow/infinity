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
import stl;
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import global_block_id;
import data_block;
import table_scan_function_data;
import block_entry;
import base_table_ref;
import column_buffer;
import block_column_entry;
import block_index;
import table_collection_entry;
import default_values;
import infinity_exception;

module physical_table_scan;

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

TableCollectionEntry *PhysicalTableScan::TableEntry() const { return base_table_ref_->table_entry_ptr_; }

SizeT PhysicalTableScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

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

    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (u64 task_id = 0, global_block_id = 0, residual_idx = 0; task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
        }
        if (residual_idx < residual) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

void PhysicalTableScan::ExecuteInternal(QueryContext *query_context,
                                        TableScanOperatorState *table_scan_operator_state) {
    if(!table_scan_operator_state->data_block_array_.empty()) {
        Error<ExecutorException>("Table scan output data block array should be empty");
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

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output_ptr->available_capacity();
    while (write_capacity > 0 && block_ids_idx < block_ids->size()) {
        u32 segment_id = block_ids->at(block_ids_idx).segment_id_;
        u16 block_id = block_ids->at(block_ids_idx).block_id_;

        BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);
        auto [row_begin, row_end] = BlockEntry::VisibleRange(current_block_entry, begin_ts, read_offset);
        auto write_size = Min(write_capacity, SizeT(row_end - row_begin));

        if (write_size > 0) {
            read_offset = row_begin;
            SizeT output_column_id{0};
            for (auto column_id : column_ids) {
                if (column_id == COLUMN_IDENTIFIER_ROW_ID) {
                    u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + read_offset;
                    output_ptr->column_vectors[output_column_id++]->AppendWith(RowID(segment_id, segment_offset), write_size);
                } else {
                    ColumnBuffer column_buffer =
                        BlockColumnEntry::GetColumnData(current_block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager());
                    output_ptr->column_vectors[output_column_id++]->AppendWith(column_buffer, read_offset, write_size);
                }
            }

            // write_size = already read size = already write size
            write_capacity -= write_size;
            read_offset += write_size;
        } else {
            // we have read all data from current block, move to next block
            ++block_ids_idx;
            read_offset = 0;
        }
    }
    if (block_ids_idx >= block_ids->size()) {
        table_scan_operator_state->SetComplete();
    }

    output_ptr->Finalize();
}

} // namespace infinity
