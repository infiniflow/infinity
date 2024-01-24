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

module bg_task;

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
    auto &segment_map = table_entry_->segment_map();
    for (SegmentID segment_id : segment_ids) {
        auto *segment_entry = segment_map.at(segment_id).get();

        segment_entry->SetCompacting(this);
        // TODO: compact segment
    }
    return nullptr;
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
    DeleteState remapped_delete;

    NewCatalog::Delete(table_entry_, 0, 0, remapped_delete);
}

} // namespace infinity