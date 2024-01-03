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

export module segment_iter;

import stl;
import catalog;
import block_iter;

namespace infinity {

export class SegmentIter {
public:
    SegmentIter(const SegmentEntry *entry, SharedPtr<Vector<SizeT>> column_ids) : entry_(entry), block_idx_(0), column_ids_(column_ids) {
        const auto& block_entries = entry->block_entries();
        if (block_entries.empty()) {
            block_iter_ = None;
        } else {
            block_iter_ = BlockIter(block_entries[block_idx_].get(), *column_ids);
        }
    }

    Optional<Vector<const void *>> Next() {
        if (!block_iter_.has_value()) {
            return None;
        }
        if (auto ret = block_iter_->Next(); ret) {
            return ret;
        }
        block_idx_++;

        const auto& block_entries = entry_->block_entries();
        if (block_idx_ >= block_entries.size()) {
            block_iter_ = None;
            return None;
        }
        block_iter_ = BlockIter(block_entries[block_idx_].get(), *column_ids_);
        return Next();
    }

private:
    const SegmentEntry *entry_;
    SizeT block_idx_;
    Optional<BlockIter> block_iter_;

    SharedPtr<Vector<SizeT>> column_ids_;
};

} // namespace infinity