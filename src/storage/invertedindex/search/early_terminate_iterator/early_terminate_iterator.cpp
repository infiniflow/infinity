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

#include <cassert>
#include <iostream>
#include <tuple>
module early_terminate_iterator;
import internal_types;
import stl;
import index_defines;
import infinity_exception;
import third_party;

namespace infinity {

Pair<RowID, float> EarlyTerminateIterator::BlockNextWithThreshold(float threshold) {
    for (RowID next_skip = doc_id_ + 1;;) {
        if (!BlockSkipTo(next_skip, threshold)) [[unlikely]] {
            return {INVALID_ROWID, 0.0F};
        }
        next_skip = std::max(next_skip, BlockMinPossibleDocID());
        assert((next_skip <= BlockLastDocID()));
        auto [success, score, id] = SeekInBlockRange(next_skip, BlockLastDocID(), threshold);
        if (success) {
            // success in SeekInBlockRange, inner doc_id_ is updated
            return {id, score};
        }
        next_skip = BlockLastDocID() + 1;
    }
}

void MultiQueryEarlyTerminateIteratorCommonPrintTree(const EarlyTerminateIterator *this_iter,
                                                     std::string_view iter_type_name,
                                                     const Vector<UniquePtr<EarlyTerminateIterator>> &children,
                                                     std::ostream &os,
                                                     const String &prefix,
                                                     bool is_final) {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << iter_type_name;
    os << " (children count: " << children.size() << ")";
    os << " (doc_freq: " << this_iter->DocFreq() << ")";
    os << " (bm25_score_upper_bound: " << this_iter->BM25ScoreUpperBound() << ")";
    os << '\n';
    const String next_prefix = prefix + (is_final ? "    " : "│   ");
    for (u32 i = 0; i + 1 < children.size(); ++i) {
        children[i]->PrintTree(os, next_prefix, false);
    }
    children.back()->PrintTree(os, next_prefix, true);
}

} // namespace infinity