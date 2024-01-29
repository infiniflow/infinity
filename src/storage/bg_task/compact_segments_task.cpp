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

module compact_segments_task;

import stl;
import catalog;
import default_values;
import infinity_exception;
import parser;
import data_access_state;
import column_vector;
import buffer_manager;
import txn;
import txn_state;
import txn_manager;
import infinity_exception;
import bg_task;
import wal;
import global_block_id;

namespace infinity {

RowID RowIDRemapper::GetNewRowID(SegmentID segment_id, BlockID block_id, BlockOffset block_offset) const {
    auto &block_vec = row_id_map_.at(GlobalBlockID(segment_id, block_id));
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

class GreedyCompactableSegmentsGenerator {
public:
    GreedyCompactableSegmentsGenerator(const Vector<SegmentEntry *> &segments, SizeT max_segment_size) : max_segment_size_(max_segment_size) {
        for (auto *segment_entry : segments) {
            segments_.emplace(segment_entry->actual_row_count(), segment_entry);
        }
    }

    Vector<SegmentEntry *> generate() {
        Vector<SegmentEntry *> result;
        do {
            result.clear();
            SizeT segment_size = max_segment_size_;

            while (true) {
                auto iter = segments_.upper_bound(segment_size);
                if (iter == segments_.begin()) {
                    break;
                }
                --iter;
                auto [row_count, segment_entry] = *iter;
                segments_.erase(iter);
                result.push_back(segment_entry);
                segment_size -= row_count;
            }
        } while (result.size() == 1);
        return result;
    }

private:
    MultiMap<SizeT, SegmentEntry *> segments_;

    const SizeT max_segment_size_;
};

CompactSegmentsTask::CompactSegmentsTask(TableEntry *table_entry, Txn *txn)
    : BGTask(BGTaskType::kCompactSegments, false), table_entry_(table_entry), txn_(txn) {}

void CompactSegmentsTask::Execute() {
    auto state = CompactSegments();
    CommitCompacts(std::move(state));
}

CompactSegmentsTaskState CompactSegmentsTask::CompactSegments() {
    CompactSegmentsTaskState state;

    Vector<SegmentEntry *> segments = PickSegmentsToCompact();
    GreedyCompactableSegmentsGenerator generator(segments, DEFAULT_SEGMENT_CAPACITY);

    while (true) {
        Vector<SegmentEntry *> to_compact_segments = generator.generate();
        if (to_compact_segments.empty()) {
            break;
        }
        auto new_segment = CompactSegmentsToOne(state.remapper_, to_compact_segments);
        state.segment_data_.emplace_back(new_segment, std::move(to_compact_segments));
    }

    // TODO: Create new index file here
    return state;
}

void CompactSegmentsTask::CommitCompacts(CompactSegmentsTaskState &&state) {
    // Save new segment, set no_delete_ts, add compact wal cmd
    SaveSegmentsData(std::move(state.segment_data_));

    // Apply the delete op commit in process of compacting
    while (!ApplyToDelete(state.remapper_)) {
        // TODO: wait some time
    }
}

void CompactSegmentsTask::AddToDelete(SegmentID segment_id, Vector<SegmentOffset> &&delete_offsets, TransactionID delete_txn_id) {
    std::unique_lock lock(mutex_);
    to_deletes_.emplace_back(ToDeleteInfo{segment_id, std::move(delete_offsets), delete_txn_id});
}

Vector<SegmentEntry *> CompactSegmentsTask::PickSegmentsToCompact() {
    Vector<SegmentEntry *> segments;
    TxnTimeStamp begin_ts = txn_->BeginTS();
    for (const auto &[segment_id, segment_entry] : table_entry_->segment_map()) {
        if (segment_entry->min_row_ts() <= begin_ts && segment_entry->deprecate_ts() >= begin_ts) {
            segment_entry->SetCompacting(this, begin_ts);
            segments.push_back(segment_entry.get());
        }
    }
    return segments;
}

SharedPtr<SegmentEntry> CompactSegmentsTask::CompactSegmentsToOne(RowIDRemapper &remapper, const Vector<SegmentEntry *> &segments) {
    SegmentID segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    auto new_segment = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn_);

    TxnTimeStamp begin_ts = txn_->BeginTS();
    SizeT column_count = table_entry_->ColumnCount();
    BufferManager *buffer_mgr = txn_->buffer_manager();

    auto *current_block_ptr = new_segment->GetLastEntry(); // why segment has at least one block? TODO: remove it
    UniquePtr<BlockEntry> current_block_entry = nullptr;
    for (auto *segment_entry : segments) {
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

void CompactSegmentsTask::SaveSegmentsData(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data) {
    Vector<WalSegmentInfo> segment_infos;
    Vector<SegmentID> old_segment_ids;

    TxnTimeStamp begin_ts = txn_->BeginTS();
    for (auto &[new_segment, old_segments] : segment_data) {
        new_segment->FlushData();
        for (auto *old_segment : old_segments) {
            old_segment->SetNoDelete(begin_ts);
        }

        u16 last_block_row_count = new_segment->block_entries().back()->row_count();
        segment_infos.emplace_back(WalSegmentInfo{*new_segment->segment_dir(),
                                                  new_segment->segment_id(),
                                                  static_cast<u16>(new_segment->block_entries().size()),
                                                  DEFAULT_BLOCK_CAPACITY, // TODO: store block capacity in segment entry
                                                  last_block_row_count});
        for (auto *old_segment : old_segments) {
            old_segment_ids.push_back(old_segment->segment_id());
        }
    }
    String db_name = *table_entry_->GetDBName();
    String table_name = *table_entry_->GetTableName();
    txn_->Compact(db_name, table_name, std::move(segment_data));
    txn_->AddWalCmd(MakeShared<WalCmdCompact>(std::move(db_name), std::move(table_name), std::move(segment_infos), std::move(old_segment_ids)));
}

bool CompactSegmentsTask::ApplyToDelete(const RowIDRemapper &remapper) {
    bool all_delete_done = true;
    Vector<RowID> row_ids;
    auto *txn_mgr = txn_->txn_mgr();
    for (const auto &delete_info : to_deletes_) {
        auto *delete_txn = txn_mgr->GetTxn(delete_info.delete_txn_id_);
        bool committed = false;
        switch (delete_txn->GetTxnState()) {
            case TxnState::kCommitted: {
                committed = true;
                break;
            }
            case TxnState::kRollbacked: {
                break;
            }
            default: {
                all_delete_done = false;
            }
        }
        if (!all_delete_done) {
            break;
        } else if (committed) {
            // remap the delete row id
            for (SegmentOffset offset : delete_info.delete_offsets) {
                RowID old_row_id(delete_info.segment_id_, offset);
                RowID new_row_id = remapper.GetNewRowID(old_row_id);
                row_ids.push_back(new_row_id);
            }
        }
    }
    if (!all_delete_done) {
        return false;
    }
    txn_->Delete(*table_entry_->GetDBName(), *table_entry_->GetTableName(), row_ids);
    return true;
}

} // namespace infinity