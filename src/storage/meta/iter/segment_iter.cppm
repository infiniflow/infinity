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

#include <utility>

export module segment_iter;

import stl;
import catalog;
import block_iter;
import buffer_manager;
import default_values;

namespace infinity {

export class SegmentIter {
public:
    SegmentIter(const SegmentEntry *entry, BufferManager *buffer_mgr, Vector<ColumnID> &&column_ids, TxnTimeStamp iterate_ts)
        : entry_(entry), buffer_mgr_(buffer_mgr), column_ids_(column_ids), iterate_ts_(iterate_ts), block_idx_(0) {
        const auto &block_entries = entry->block_entries();
        if (block_entries.empty()) {
            block_iter_ = None;
        } else {
            block_iter_ = BlockIter(block_entries[block_idx_].get(), buffer_mgr, column_ids_, iterate_ts_);
        }
    }

    Optional<Pair<Vector<const void *>, SegmentOffset>> Next() {
        if (!block_iter_.has_value()) {
            return None;
        }
        if (auto ret = block_iter_->Next(); ret) {
            auto &[vec, offset] = *ret;
            return std::make_pair(std::move(vec), SegmentOffset(offset + block_idx_ * DEFAULT_BLOCK_CAPACITY));
        }
        block_idx_++;

        const auto &block_entries = entry_->block_entries();
        if (block_idx_ >= block_entries.size()) {
            block_iter_ = None;
            return None;
        }
        // FIXME: segment_entry should store the block capacity
        block_iter_ = BlockIter(block_entries[block_idx_].get(), buffer_mgr_, column_ids_, iterate_ts_);
        return Next();
    }

private:
    const SegmentEntry *const entry_;
    BufferManager *const buffer_mgr_;
    const Vector<ColumnID> column_ids_;
    const TxnTimeStamp iterate_ts_;

    BlockID block_idx_;
    Optional<BlockIter> block_iter_;
};

export template <typename DataType>
class OneColumnIterator {
public:
    using LabelType = SegmentOffset;

    OneColumnIterator(const SegmentEntry *entry, BufferManager *buffer_mgr, ColumnID column_id, TxnTimeStamp iterate_ts)
        : segment_iter_(entry, buffer_mgr, Vector<ColumnID>{column_id}, iterate_ts) {}

    Optional<Pair<const DataType *, SegmentOffset>> Next() {
        if (auto ret = segment_iter_.Next(); ret) {
            auto [vec, offset] = *ret;
            auto v_ptr = reinterpret_cast<const DataType *>(vec[0]);
            return std::make_pair(v_ptr, offset);
        }
        return None;
    }

private:
    SegmentIter segment_iter_;
};

} // namespace infinity