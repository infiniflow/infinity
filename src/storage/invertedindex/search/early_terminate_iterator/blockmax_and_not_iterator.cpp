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

#include <cassert>
#include <vector>
module blockmax_and_not_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;

namespace infinity {

// simple case code for Term and "AND"
Pair<RowID, float> BlockMaxAndNotIterator::NextWithThreshold(float threshold) { return {}; }

// simple case code for Term and "AND"
Pair<RowID, float> BlockMaxAndNotIterator::BlockNextWithThreshold(float threshold) {
    for (RowID next_skip = doc_id_ + 1;;) {
        if (!BlockSkipTo(next_skip, threshold)) [[unlikely]] {
            return {INVALID_ROWID, 0.0F};
        }
        next_skip = std::max(next_skip, BlockMinPossibleDocID());
        auto [success, score, id] = SeekInBlockRange(next_skip, threshold, BlockLastDocID());
        if (success) {
            // success in SeekInBlockRange, inner doc_id_ is updated
            return {id, score};
        }
        next_skip = BlockLastDocID() + 1;
    }
}

Tuple<bool, float, RowID> BlockMaxAndNotIterator::SeekInBlockRange(RowID doc_id, float threshold, RowID doc_id_no_beyond) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        if (doc_id > block_end) [[unlikely]] {
            return {false, 0.0F, INVALID_ROWID};
        }
        auto [success, score, id] = inner_iterators_[0]->SeekInBlockRange(doc_id, threshold, block_end);
        if (!success) {
            return {false, 0.0F, INVALID_ROWID};
        }
        bool all_match = true;
        for (u32 i = 1; i < inner_iterators_.size(); ++i) {
            if (inner_iterators_[i]->Seek(id)) {
                all_match = false;
                break;
            }
        }
        if (all_match) {
            doc_id_ = id;
            return {true, score, id};
        }
        doc_id = id + 1;
    }
}

} // namespace infinity