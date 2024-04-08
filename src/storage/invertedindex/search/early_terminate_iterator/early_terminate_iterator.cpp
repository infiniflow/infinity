//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

#include <tuple>
module early_terminate_iterator;
import internal_types;
import stl;
import index_defines;

namespace infinity {

/*
// simple case code for Term and "AND"
Pair<RowID, float> EarlyTerminateIterator::NextWithThreshold(float threshold) {
    while (true) {
        RowID next_doc = Next();
        if (next_doc == INVALID_ROWID) [[unlikely]] {
            return {INVALID_ROWID, 0.0F};
        }
        if (float score = BM25Score(); score >= threshold) {
            return {next_doc, score};
        }
    }
}

// simple case code for Term and "AND"
Pair<RowID, float> EarlyTerminateIterator::BlockNextWithThreshold(float threshold) {
    for (RowID next_skip = doc_id_ + 1;;) {
        if (!BlockSkipTo(next_skip, threshold)) [[unlikely]] {
            return {INVALID_ROWID, 0.0F};
        }
        next_skip = std::max(next_skip, BlockMinPossibleDocID());
        auto [success, score, id] = SeekInBlockRange(next_skip, threshold);
        if (success) {
            // success in SeekInBlockRange, inner doc_id_ is updated
            return {id, score};
        }
        next_skip = BlockLastDocID() + 1;
    }
}
*/

} // namespace infinity