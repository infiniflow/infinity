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
                auto iter = segments_.lower_bound(segment_size);
                if (iter == segments_.end()) {
                    break;
                }
                auto [row_count, segment_id] = *iter;
                segments_.erase(iter);
                result.push_back(segment_id);
                segment_size -= row_count;
            }
        } while (result.size() == 1);
        return result;
    }

private:
    Map<SizeT, SegmentID> segments_;

    const SizeT max_segment_size_;
};

CompactSegmentsTask::CompactSegmentsTask(TableEntry *table_entry)
    : BGTask(BGTaskType::kCompactSegments, false), table_entry_(table_entry), max_segment_id_(NewCatalog::GetMaxSegmentID(table_entry_)) {

    for (const auto &[segment_id, segment] : table_entry_->segment_map()) {
        if (segment_id > max_segment_id_) {
            continue;
        }
        segment_entries_.emplace_back(segment.get());
    }
}

void CompactSegmentsTask::Execute() {
    // TODO: allocate a new table_entry

    Vector<Pair<SegmentID, SizeT>> segments;
    for (const auto *segment_entry : segment_entries_) {
        segments.emplace_back(segment_entry->segment_id(), segment_entry->remain_row_count());
    }
    GreedySegGenerator generator(segments, DEFAULT_SEGMENT_CAPACITY);
    while (true) {
        Vector<SegmentID> to_compact_segments = generator();
        if (to_compact_segments.empty()) {
            break;
        }
        auto segment_entry = CompactSegments(std::move(to_compact_segments));
        // TODO: add segment to table_entry
    }

    ApplyAllToDelete();
}

void CompactSegmentsTask::AddToDelete(SegmentID segment_id, HashMap<BlockID, Vector<BlockOffset>> block_row_hashmap) {
    std::lock_guard<std::mutex> lock(mtx_);
    to_deletes_.emplace_back(segment_id, std::move(block_row_hashmap));
}

SharedPtr<SegmentEntry> CompactSegmentsTask::CompactSegments(Vector<SegmentID> segment_ids) {
    Txn *txn = nullptr;
    SegmentID segment_id = NewCatalog::GetNextSegmentID(table_entry_);
    auto new_segment = SegmentEntry::NewSegmentEntry(table_entry_, segment_id, txn);

    TxnTimeStamp ts = 0;
    SizeT column_count = table_entry_->ColumnCount();
    BufferManager *buffer_mgr = nullptr;

    auto &segment_map = table_entry_->segment_map();
    auto current_block_entry = BlockEntry::NewBlockEntry(new_segment.get(), 0, 0, column_count, txn);
    for (SegmentID segment_id : segment_ids) {
        auto *segment_entry = segment_map.at(segment_id).get();
        segment_entry->SetCompacting(this);

        for (auto &block_entry : segment_entry->block_entries()) {
            SizeT read_offset = 0;
            Vector<ColumnVector> input_column_vectors;
            for (ColumnID column_id = 0; column_id < column_count; ++column_id) {
                auto *column_block_entry = block_entry->GetColumnBlockEntry(column_id);
                input_column_vectors.emplace_back(column_block_entry->GetColumnVector(buffer_mgr));
            }
            while (true) {
                auto [row_begin, row_end] = block_entry->GetVisibleRange(ts, read_offset);
                SizeT read_size = row_end - row_begin;
                if (read_size == 0) {
                    break;
                }
                auto BlockEntryAppend = [&](SizeT row_begin, SizeT read_size) {
                    for (ColumnID column_id = 0; column_id < column_count; ++column_id) {
                        auto *current_column_block_entry = current_block_entry->GetColumnBlockEntry(column_id);
                        current_column_block_entry->Append(&input_column_vectors[column_id], row_begin, read_size, buffer_mgr);
                    }
                    RowID new_row_id(new_segment->segment_id(),
                                     current_block_entry->block_id() * DEFAULT_BLOCK_CAPACITY + current_block_entry->row_count());
                    remapper_.AddMap(segment_id, block_entry->block_id(), row_begin, new_row_id);
                };

                if (read_size + current_block_entry->row_count() > current_block_entry->row_capacity()) {
                    SizeT read_size1 = current_block_entry->row_capacity() - current_block_entry->row_count();
                    BlockEntryAppend(row_begin, read_size1);
                    row_begin += read_size1;
                    new_segment->AppendBlockEntry(std::move(current_block_entry));
                    current_block_entry = BlockEntry::NewBlockEntry(new_segment.get(), 0, 0, column_count, txn);
                }
                BlockEntryAppend(row_begin, read_size);
            }
        }
    }
    if (current_block_entry->row_count() > 0) {
        new_segment->AppendBlockEntry(std::move(current_block_entry));
    }

    return new_segment;
}

void CompactSegmentsTask::ApplyAllToDelete() {
    mtx_.lock();
    SizeT to_delete_count = to_deletes_.size();
    auto iter = to_deletes_.begin();
    while (iter != to_deletes_.end()) {
        mtx_.unlock();
        const auto &[segment_id, to_delete] = *iter;
        ApplyOneToDelete(segment_id, to_delete);

        mtx_.lock();
        ++iter;
    }
    mtx_.unlock();
    for (auto *segment_entry : segment_entries_) {
        segment_entry->SetDeprecated();
    }
    mtx_.lock();
    SizeT to_delete_count2 = to_deletes_.size();
    mtx_.unlock();
    if (to_delete_count != to_delete_count2) {
        auto iter = to_deletes_.end();
        for (SizeT i = 0; i < to_delete_count2 - to_delete_count; ++i) {
            --iter;
        }
        while (iter != to_deletes_.end()) {
            const auto &[segment_id, to_delete] = *iter;
            ApplyOneToDelete(segment_id, to_delete);

            ++iter;
        }
    }
}

void CompactSegmentsTask::ApplyOneToDelete(SegmentID segment_id, const HashMap<BlockID, Vector<BlockOffset>> &to_delete) {
    // TODO: remap the delete row id
    DeleteState remapped_delete_state;
    auto &delete_rows = remapped_delete_state.rows_[segment_id];
    for (const auto &[block_id, block_offsets] : to_delete) {
        for (BlockOffset block_offset : block_offsets) {
            RowID new_row_id = remapper_.GetNewRowID(segment_id, block_id, block_offset);
            BlockID new_block_id = new_row_id.segment_offset_ / DEFAULT_BLOCK_CAPACITY;
            BlockOffset new_block_offset = new_row_id.segment_offset_ % DEFAULT_BLOCK_CAPACITY;
            delete_rows[new_block_id].push_back(new_block_offset);
        }
    }

    NewCatalog::Delete(table_entry_, 0, 0, remapped_delete_state);
}

} // namespace infinity