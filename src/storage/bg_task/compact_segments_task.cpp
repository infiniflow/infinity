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

import stl;
import catalog;
import default_values;
import infinity_exception;
import parser;
import data_access_state;
import column_vector;
import buffer_manager;
import txn;
import infinity_exception;
import bg_task;
import wal;

module compact_segments_task;

namespace infinity {

class GreedySegGenerator {
public:
    GreedySegGenerator(const Vector<Pair<SegmentID, SizeT>> &segments, SizeT max_segment_size) : max_segment_size_(max_segment_size) {
        for (const auto &[segment_id, row_count] : segments) {
            segments_.emplace(row_count, segment_id);
        }
    }

    Vector<SegmentID> operator()() {
        Vector<SegmentID> result;
        do {
            result.clear();
            SizeT segment_size = max_segment_size_;

            while (true) {
                auto iter = segments_.upper_bound(segment_size);
                if (iter == segments_.begin()) {
                    break;
                }
                --iter;
                auto [row_count, segment_id] = *iter;
                segments_.erase(iter);
                result.push_back(segment_id);
                segment_size -= row_count;
            }
        } while (result.size() == 1);
        return result;
    }

private:
    MultiMap<SizeT, SegmentID> segments_;

    const SizeT max_segment_size_;
};

CompactSegmentsTask::CompactSegmentsTask(TableEntry *table_entry, Txn *txn)
    : BGTask(BGTaskType::kCompactSegments, false), table_entry_(table_entry), txn_(txn) {}

void CompactSegmentsTask::Execute() {
    Execute1();
    Execute2();
}

void CompactSegmentsTask::Execute1() {
    Vector<Pair<SegmentID, SizeT>> segments;
    TxnTimeStamp begin_ts = txn_->BeginTS();
    // scan the segments
    for (const auto &[segment_id, segment_entry] : table_entry_->segment_map()) {
        if (segment_entry->min_row_ts() <= begin_ts && segment_entry->max_row_ts() >= begin_ts) {
            segments.emplace_back(segment_id, segment_entry->remain_row_count());
        }
    }

    GreedySegGenerator generator(segments, DEFAULT_SEGMENT_CAPACITY);

    while (true) {
        Vector<SegmentID> to_compact_segments = generator();
        if (to_compact_segments.empty()) {
            break;
        }
        auto new_segment = CompactSegments(to_compact_segments);
        segment_data_.emplace_back(new_segment, std::move(to_compact_segments));
        new_segments_.emplace(new_segment->segment_id(), new_segment);
    }

    // TODO: Create new index file here
}

void CompactSegmentsTask::Execute2() {
    SaveSegmentsData(std::move(segment_data_));
    // Apply the delete op commit in process of compacting

    TxnTimeStamp begin_ts = txn_->BeginTS();
    Vector<SegmentEntry *> deprecated_segments;
    for (const auto &[segment_id, segment_entry] : table_entry_->segment_map()) {
        if (segment_entry->min_row_ts() <= begin_ts && segment_entry->max_row_ts() >= begin_ts) {
            deprecated_segments.push_back(segment_entry.get());
        }
    }
    ApplyToDelete(deprecated_segments);
}

void CompactSegmentsTask::AddToDelete(SegmentID segment_id, HashMap<BlockID, Vector<BlockOffset>> &&block_row_hashmap, TxnTimeStamp commit_ts) {
    to_deletes_.emplace_back(ToDeleteInfo{segment_id, std::move(block_row_hashmap), commit_ts});
}

// Set old segment deprecated and add compact_wal_cmd, import_wal_cmd
void CompactSegmentsTask::SaveSegmentsData(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentID>>> &&data) {
    TxnTimeStamp begin_ts = txn_->BeginTS();
    for (auto &[new_segment, old_segment_ids] : data) {
        new_segment->FlushData();

        auto *txn_store = txn_->GetTxnTableStore(table_entry_);
        txn_store->Import(new_segment);

        const auto &block_entries = new_segment->block_entries();
        SizeT block_entries_size = block_entries.size();
        u16 last_block_row_count = block_entries.back()->row_count();
        String db_name = *table_entry_->GetDBName();
        String table_name = *table_entry_->GetTableName();
        txn_->AddWalCmd(MakeShared<WalCmdImport>(db_name,
                                                 table_name,
                                                 *new_segment->segment_dir(),
                                                 new_segment->segment_id(),
                                                 block_entries_size,
                                                 DEFAULT_BLOCK_CAPACITY, // TODO: store block capacity in segment entry
                                                 last_block_row_count));

        for (SegmentID segment_id : old_segment_ids) {
            auto *segment_entry = table_entry_->segment_map().at(segment_id).get();
            segment_entry->SetDeprecated(begin_ts);
        }

        txn_->AddWalCmd(MakeShared<WalCmdCompact>(std::move(db_name), std::move(table_name), std::move(old_segment_ids)));
    }
}

SharedPtr<SegmentEntry> CompactSegmentsTask::CompactSegments(const Vector<SegmentID> &segment_ids) {
    SegmentID segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    auto new_segment = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn_);

    TxnTimeStamp begin_ts = txn_->BeginTS();
    SizeT column_count = table_entry_->ColumnCount();
    BufferManager *buffer_mgr = txn_->buffer_manager();

    auto &segment_map = table_entry_->segment_map();

    auto *current_block_entry = new_segment->GetLastEntry(); // why segment has at least one block? TODO: remove it
    UniquePtr<BlockEntry> p = nullptr;
    for (SegmentID segment_id : segment_ids) {
        auto *segment_entry = segment_map.at(segment_id).get();
        segment_entry->SetCompacting(this, begin_ts);

        for (auto &block_entry : segment_entry->block_entries()) {
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

                auto block_entry_append = [&](SizeT row_begin, SizeT read_size) {
                    current_block_entry->AppendBlock(input_column_vectors, row_begin, read_size, buffer_mgr);
                    RowID new_row_id(new_segment->segment_id(),
                                     current_block_entry->block_id() * DEFAULT_BLOCK_CAPACITY + current_block_entry->row_count());
                    remapper_.AddMap(segment_id, block_entry->block_id(), row_begin, new_row_id);
                    read_offset = row_begin + read_size;
                };

                if (read_size + current_block_entry->row_count() > current_block_entry->row_capacity()) {
                    SizeT read_size1 = current_block_entry->row_capacity() - current_block_entry->row_count();
                    block_entry_append(row_begin, read_size1);
                    row_begin += read_size1;
                    read_size -= read_size1;
                    if (p.get() != nullptr) {
                        new_segment->AppendBlockEntry(std::move(p));
                    }
                    BlockID next_block_id = new_segment->block_entries().size();
                    p = BlockEntry::NewBlockEntry(new_segment.get(), next_block_id, 0, column_count, txn_);
                    current_block_entry = p.get();
                }
                block_entry_append(row_begin, read_size);
            }
        }
    }
    if (p.get() != nullptr && p->row_count() > 0) {
        new_segment->AppendBlockEntry(std::move(p));
    }

    for (auto &block_entry : new_segment->block_entries()) {
        new_segment->IncreaseRowCount(block_entry->row_count());
    }

    return new_segment;
}

void CompactSegmentsTask::ApplyToDelete(const Vector<SegmentEntry *> &segment_entries) {
    for (auto &delete_info : to_deletes_) {
        // remap the delete row id
        DeleteState remapped_delete_state;
        for (const auto &[block_id, block_offsets] : delete_info.block_row_hashmap_) {
            for (BlockOffset block_offset : block_offsets) {
                RowID new_row_id = remapper_.GetNewRowID(delete_info.segment_id_, block_id, block_offset);
                BlockID new_block_id = new_row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
                BlockOffset new_block_offset = new_row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
                auto &delete_rows = remapped_delete_state.rows_[new_row_id.segment_id_];
                delete_rows[new_block_id].push_back(new_block_offset);
            }
        }

        // Add delete state to txn_state
        // NewCatalog::Delete(table_entry_, txn_->TxnID(), delete_info.commit_ts_, remapped_delete_state);
        for (const auto [segment_id, delete_rows] : remapped_delete_state.rows_) {
            new_segments_[segment_id]->DeleteData(txn_->TxnID(), delete_info.commit_ts_, delete_rows);
        }
    }
    to_deletes_.clear();
}

} // namespace infinity