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

#include <vector>

module physical_compact;

import stl;
import internal_types;
import operator_state;
import base_table_ref;
import block_index;
import column_vector;
import catalog;
import table_entry;
import segment_entry;
import block_entry;
import compact_state_data;

namespace infinity {

bool PhysicalCompact::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *state = static_cast<CompactOperatorState *>(operator_state);

    auto *txn = query_context->GetTxn();
    auto *buffer_mgr = query_context->storage()->buffer_manager();
    TxnTimeStamp begin_ts = txn->BeginTS();

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    SizeT column_count = table_entry->ColumnCount();

    if (state->result_segment_entry_.get() == nullptr) {
        state->result_segment_entry_ = SegmentEntry::NewSegmentEntry(table_entry, Catalog::GetNextSegmentID(table_entry), txn);
    }
    SegmentEntry *new_segment = state->result_segment_entry_.get();
    SegmentID new_segment_id = new_segment->segment_id();
    CompactStateData &compact_state_data = state->compact_state_data_;

    UniquePtr<BlockEntry> new_block = BlockEntry::NewBlockEntry(new_segment, new_segment->GetNextBlockID(), 0 /*checkpoint_ts*/, column_count, txn);
    const SizeT block_capacity = new_block->row_capacity();

    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        // auto *segment_entry = block_index->segment_index_.at(segment_id);
        // SegmentOffset segment_offset = block_info.segment_offset_;

        for (const auto *block_entry : segment_info.block_map_) {
            BlockID block_id = block_entry->block_id();
            Vector<ColumnVector> input_column_vectors;
            for (ColumnID column_id = 0; column_id < column_count; ++column_id) {
                auto *column_block_entry = block_entry->GetColumnBlockEntry(column_id);
                input_column_vectors.emplace_back(column_block_entry->GetColumnVector(buffer_mgr));
            }
            SizeT read_offset = 0;
            while (true) {
                auto [row_begin, row_end] = block_entry->GetVisibleRange(begin_ts, read_offset);
                SizeT read_size = row_end - row_begin;
                if (read_size == 0) {
                    break;
                }

                auto block_entry_append = [&](SizeT row_begin, SizeT read_size1) {
                    if (read_size1 == 0) {
                        return;
                    }
                    new_block->AppendBlock(input_column_vectors, row_begin, read_size1, buffer_mgr);
                    RowID new_row_id(new_segment_id, new_block->block_id() * block_capacity + new_block->row_count());
                    compact_state_data.AddMap(segment_id, block_id, row_begin, new_row_id);
                    read_offset = row_begin + read_size1;
                };

                if (read_size + new_block->row_count() > block_capacity) {
                    SizeT read_size1 = block_capacity - new_block->row_count();
                    block_entry_append(row_begin, read_size1);
                    row_begin += read_size1;
                    read_size -= read_size1;
                    new_segment->AppendBlockEntry(std::move(new_block));

                    new_block = BlockEntry::NewBlockEntry(new_segment, new_segment->GetNextBlockID(), 0, column_count, txn);
                }
                block_entry_append(row_begin, read_size);
            }
        }
    }
    if (new_block->row_count() > 0) {
        new_segment->AppendBlockEntry(std::move(new_block));
    }

    return true;
}

SizeT PhysicalCompact::TaskletCount() {
    //
    return 0;
}

} // namespace infinity