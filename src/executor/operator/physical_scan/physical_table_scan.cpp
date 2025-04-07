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
import third_party;
import logger;
import column_vector;
import logical_type;
import meta_info;
import block_entry;

import new_txn;
import block_meta;
import new_catalog;
import column_meta;
import status;

namespace infinity {

void PhysicalTableScan::Init(QueryContext* query_context) {}

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

SizeT PhysicalTableScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

Vector<SizeT> &PhysicalTableScan::ColumnIDs() const {
    if (!add_row_id_)
        return base_table_ref_->column_ids_;
    if (!column_ids_.empty())
        return column_ids_;
    column_ids_ = base_table_ref_->column_ids_;
    column_ids_.push_back(COLUMN_IDENTIFIER_ROW_ID);
    return column_ids_;
}

void PhysicalTableScan::ExecuteInternal(QueryContext *query_context, TableScanOperatorState *table_scan_operator_state) {
    if (!table_scan_operator_state->data_block_array_.empty()) {
        String error_message = "Table scan output data block array should be empty";
        UnrecoverableError(error_message);
    }

    table_scan_operator_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
    DataBlock *output_ptr = table_scan_operator_state->data_block_array_.back().get();
    output_ptr->Init(*GetOutputTypes());

    TableScanFunctionData *table_scan_function_data_ptr = table_scan_operator_state->table_scan_function_data_.get();
    const BlockIndex *block_index = table_scan_function_data_ptr->block_index_;
    Vector<GlobalBlockID> *block_ids = table_scan_function_data_ptr->global_block_ids_.get();
    const Vector<SizeT> &column_ids = table_scan_function_data_ptr->column_ids_;
    u64 &block_ids_idx = table_scan_function_data_ptr->current_block_ids_idx_;
    SizeT block_ids_count = block_ids->size();
    if (block_ids_idx >= block_ids_count) {
        // No data or all data is read
        table_scan_operator_state->SetComplete();
        return;
    }

    TxnTimeStamp begin_ts;
    bool use_new_catalog = query_context->global_config()->UseNewCatalog();
    if (!use_new_catalog) {
        begin_ts = query_context->GetTxn()->BeginTS();
    } else {
        begin_ts = query_context->GetNewTxn()->BeginTS();
    }

#ifdef INFINITY_DEBUG
    // This part has performance issue
    {
        String out;
        for (auto &global_block_id : *block_ids) {
            out += fmt::format("({},{}) ", global_block_id.segment_id_, global_block_id.block_id_);
        }
        LOG_TRACE(fmt::format("TableScan: block_ids: {}", out));
    }
#endif

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output_ptr->available_capacity();
    while (block_ids_idx < block_ids_count) {
        u32 segment_id = block_ids->at(block_ids_idx).segment_id_;
        u16 block_id = block_ids->at(block_ids_idx).block_id_;

        if (use_new_catalog) {
            BlockMeta *current_block_meta = block_index->GetBlockMeta(segment_id, block_id);
            Optional<NewTxnGetVisibleRangeState> &range_state = table_scan_function_data_ptr->get_visible_range_state_;
            if (!range_state) {
                // TODO
                range_state = NewTxnGetVisibleRangeState();
                Status status = NewCatalog::GetBlockVisibleRange(*current_block_meta, begin_ts, *range_state);
                if (!status.ok()) {
                    RecoverableError(status);
                }
            }

            Pair<BlockOffset, BlockOffset> visible_range;
            bool has_next = range_state->Next(visible_range);
            if (!has_next || visible_range.first == visible_range.second) {
                // we have read all data from current block, move to next block
                ++block_ids_idx;
                range_state.reset();
                continue;
            }
            if (write_capacity == 0) {
                // output is full
                break;
            }

            auto write_size = std::min(write_capacity, SizeT(visible_range.second - visible_range.first));
            SizeT output_column_id{0};
            for (auto column_id : column_ids) {
                switch (column_id) {
                    case COLUMN_IDENTIFIER_ROW_ID: {
                        u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + visible_range.first;
                        output_ptr->column_vectors[output_column_id]->AppendWith(RowID(segment_id, segment_offset), write_size);
                        break;
                    }
                    case COLUMN_IDENTIFIER_CREATE: {
                        UnrecoverableError("Not implemented yet");
                        break;
                    }
                    case COLUMN_IDENTIFIER_DELETE: {
                        UnrecoverableError("Not implemented yet");
                        break;
                    }
                    default: {
                        ColumnVector column_vector;
                        ColumnMeta column_meta(column_id, *current_block_meta);
                        SizeT row_cnt = range_state->block_offset_end();
                        Status status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, column_vector);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        output_ptr->column_vectors[output_column_id]->AppendWith(column_vector, visible_range.first, write_size);
                    }
                }
                ++output_column_id;
            }

            // write_size = already read size = already write size
            write_capacity -= write_size;
            range_state->SetBlockOffsetBegin(visible_range.first + write_size);
            continue;
        }
        SizeT &read_offset = table_scan_function_data_ptr->current_read_offset_;
        BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);
        if (read_offset == 0) {
            // new block, check FastRoughFilter
            const auto &fast_rough_filter = *current_block_entry->GetFastRoughFilter();
            if (fast_rough_filter_evaluator_ and !fast_rough_filter_evaluator_->Evaluate(begin_ts, fast_rough_filter)) {
                // skip this block
                LOG_TRACE(fmt::format("TableScan: block_ids_idx: {}, block_ids.size(): {}, skipped after apply FastRoughFilter",
                                      block_ids_idx,
                                      block_ids_count));
                ++block_ids_idx;
                continue;
            } else {
                LOG_TRACE(fmt::format("TableScan: block_ids_idx: {}, block_ids.size(): {}, not skipped after apply FastRoughFilter",
                                      block_ids_idx,
                                      block_ids_count));
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
        auto *buffer_mgr = query_context->storage()->buffer_manager();
        for (auto column_id : column_ids) {
            switch (column_id) {
                case COLUMN_IDENTIFIER_ROW_ID: {
                    u32 segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + read_offset;
                    output_ptr->column_vectors[output_column_id]->AppendWith(RowID(segment_id, segment_offset), write_size);
                    break;
                }
                case COLUMN_IDENTIFIER_CREATE: {
                    ColumnVector create_ts_vec = current_block_entry->GetCreateTSVector(buffer_mgr, read_offset, write_size);
                    output_ptr->column_vectors[output_column_id]->AppendWith(create_ts_vec);
                    break;
                }
                case COLUMN_IDENTIFIER_DELETE: {
                    ColumnVector delete_ts_vec = current_block_entry->GetDeleteTSVector(buffer_mgr, read_offset, write_size);
                    output_ptr->column_vectors[output_column_id]->AppendWith(delete_ts_vec);
                    break;
                }
                default: {
                    ColumnVector column_vector = current_block_entry->GetConstColumnVector(buffer_mgr, column_id);
                    output_ptr->column_vectors[output_column_id]->AppendWith(column_vector, read_offset, write_size);
                }
            }
            ++output_column_id;
        }

        // write_size = already read size = already write size
        write_capacity -= write_size;
        read_offset += write_size;
    }

    LOG_TRACE(fmt::format("TableScan: block_ids_idx: {}, block_ids.size(): {}", block_ids_idx, block_ids_count));

    if (block_ids_idx >= block_ids_count) {
        table_scan_operator_state->SetComplete();
    }

    output_ptr->Finalize();
}

} // namespace infinity
