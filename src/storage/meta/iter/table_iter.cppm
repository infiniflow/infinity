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
import segment_iter;
import table_collection_entry;
import segment_entry;

export module table_iter;

namespace infinity {

export class TableIter {
private:
    using SegmentIdx = Map<u32, SharedPtr<SegmentEntry>>::const_iterator;

    TableIter(const TableCollectionEntry *entry, SegmentIdx idx, Optional<SegmentIter> iter)
        : entry_(entry), segment_idx_(Move(idx)), segment_iter_(Move(iter)) {}

public:
    TableIter Make(const TableCollectionEntry *entry, const Vector<SizeT> &column_ids) {
        if (entry->segment_map_.empty()) {
            return TableIter(entry, entry->segment_map_.end(), None);
        } else {
            return TableIter(entry, entry->segment_map_.begin(), SegmentIter(entry->segment_map_.begin()->second.get(), column_ids));
        }
    }

    Optional<Vector<const void *>> Next() {
        if (!segment_iter_.has_value()) {
            return None;
        }
        if (auto ret = segment_iter_->Next(); ret) {
            return ret;
        }
        if (segment_idx_ == entry_->segment_map_.end()) {
            segment_iter_ = None;
            return None;
        }
        segment_idx_++;
        if (segment_idx_ == entry_->segment_map_.end()) {
            segment_iter_ = SegmentIter(entry_->unsealed_segment_, segment_iter_->GetColumnIds());
        } else {
            segment_iter_ = SegmentIter(segment_idx_->second.get(), segment_iter_->GetColumnIds());
        }
        return Next();
    }

private:
    const TableCollectionEntry *entry_;
    SegmentIdx segment_idx_;
    Optional<SegmentIter> segment_iter_;
};

} // namespace infinity