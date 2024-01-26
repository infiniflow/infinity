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

#include <algorithm>
#include <vector>

module compact_segments_task;

namespace infinity {

RowID RowIDRemapper::GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const {
    auto &segment_map = row_id_map_.at(segment_id);
    auto &block_vec = segment_map.at(block_id);
    auto iter = std::upper_bound(block_vec.begin(),
                                 block_vec.end(),
                                 block_offset,
                                 [](BlockOffset block_offset, const Pair<BlockOffset, RowID> &pair) { return block_offset < pair.first; } // NOLINT
    );
    if (iter == block_vec.begin()) {
        UnrecoverableError("RowID not found");
    }
    --iter;
    RowID rtn = iter->second;
    rtn.segment_offset_ += iter->first - block_offset;
    return rtn;
}

class GreedyCompactableSegsGenerator {
public:
    GreedyCompactableSegsGenerator(const Vector<Pair<SegmentID, SizeT>> &segments, SizeT max_segment_size) : max_segment_size_(max_segment_size) {
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
    auto state = CompactSegs();
    CommitCompacts(std::move(state));
}

CompactSegmentsTaskState CompactSegmentsTask::CompactSegs() {
    CompactSegmentsTaskState state;

    Vector<Pair<SegmentID, SizeT>> segments;
    TxnTimeStamp begin_ts = txn_->BeginTS();
    // scan the segments
    for (const auto &[segment_id, segment_entry] : table_entry_->segment_map()) {
        if (segment_entry->min_row_ts() <= begin_ts && segment_entry->max_row_ts() >= begin_ts) {
            segments.emplace_back(segment_id, segment_entry->remain_row_count());
        }
    }
    GreedyCompactableSegsGenerator generator(segments, DEFAULT_SEGMENT_CAPACITY);

    while (true) {
        Vector<SegmentID> to_compact_segments = generator();
        if (to_compact_segments.empty()) {
            break;
        }
        auto new_segment = CompactSegsIntoOne(state.remapper_, to_compact_segments);
        state.segment_data_.emplace_back(new_segment, std::move(to_compact_segments));
    }

    // TODO: Create new index file here
    return state;
}

void CompactSegmentsTask::CommitCompacts(CompactSegmentsTaskState &&state) {
    // Save the compacted segments data, set segment deprecated and add wal cmd
    SaveSegmentsData(std::move(state.segment_data_));

    // Apply the delete op commit in process of compacting
    ApplyToDelete(state.remapper_);
}

void CompactSegmentsTask::AddToDelete(SegmentID segment_id, HashMap<BlockID, Vector<BlockOffset>> &&block_row_hashmap, TxnTimeStamp commit_ts) {
    to_deletes_.emplace_back(ToDeleteInfo{segment_id, std::move(block_row_hashmap), commit_ts});
}

SharedPtr<SegmentEntry> CompactSegmentsTask::CompactSegsIntoOne(RowIDRemapper &remapper, const Vector<SegmentID> &segment_ids) {
    SegmentID segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    auto new_segment = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn_);

    TxnTimeStamp begin_ts = txn_->BeginTS();
    SizeT column_count = table_entry_->ColumnCount();
    BufferManager *buffer_mgr = txn_->buffer_manager();

    auto &segment_map = table_entry_->segment_map();

    auto *current_block_ptr = new_segment->GetLastEntry(); // why segment has at least one block? TODO: remove it
    UniquePtr<BlockEntry> current_block_entry = nullptr;
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
                    current_block_ptr->AppendBlock(input_column_vectors, row_begin, read_size, buffer_mgr);
                    RowID new_row_id(new_segment->segment_id(),
                                     current_block_ptr->block_id() * DEFAULT_BLOCK_CAPACITY + current_block_ptr->row_count());
                    remapper.AddMap(segment_id, block_entry->block_id(), row_begin, new_row_id);
                    read_offset = row_begin + read_size;
                };

                if (read_size + current_block_ptr->row_count() > current_block_ptr->row_capacity()) {
                    SizeT read_size1 = current_block_ptr->row_capacity() - current_block_ptr->row_count();
                    block_entry_append(row_begin, read_size1);
                    row_begin += read_size1;
                    read_size -= read_size1;
                    if (current_block_entry.get() != nullptr) {
                        new_segment->AppendBlockEntry(std::move(current_block_entry));
                    }
                    BlockID next_block_id = new_segment->block_entries().size();
                    current_block_entry = BlockEntry::NewBlockEntry(new_segment.get(), next_block_id, 0, column_count, txn_);
                    current_block_ptr = current_block_entry.get();
                }
                block_entry_append(row_begin, read_size);
            }
        }
    }
    if (current_block_entry.get() != nullptr && current_block_entry->row_count() > 0) {
        new_segment->AppendBlockEntry(std::move(current_block_entry));
    }

    // TODO: Increase row count here is not good
    for (auto &block_entry : new_segment->block_entries()) {
        new_segment->IncreaseRowCount(block_entry->row_count());
    }

    return new_segment;
}

// Set old segment deprecated and add compact_wal_cmd, import_wal_cmd
void CompactSegmentsTask::SaveSegmentsData(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentID>>> &&segment_data) {
    TxnTimeStamp begin_ts = txn_->BeginTS();
    for (auto &[new_segment, old_segment_ids] : segment_data) {
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

void CompactSegmentsTask::ApplyToDelete(const RowIDRemapper &remapper) {
    Vector<RowID> row_ids;
    for (const auto &delete_info : to_deletes_) {
        // remap the delete row id
        for (const auto &[block_id, block_offsets] : delete_info.block_row_hashmap_) {
            for (BlockOffset block_offset : block_offsets) {
                RowID new_row_id = remapper.GetNewRowID(delete_info.segment_id_, block_id, block_offset);
                row_ids.push_back(new_row_id);
            }
        }
    }
    txn_->Delete(*table_entry_->GetDBName(), *table_entry_->GetTableName(), row_ids);
}

} // namespace infinity