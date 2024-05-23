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


bool BlockMaxAndNotIterator::NextShallow(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    return inner_iterators_[0]->NextShallow(doc_id);
}

bool BlockMaxAndNotIterator::Next(RowID doc_id){
    assert(doc_id != INVALID_ROWID);
    assert(doc_id_ == INVALID_ROWID || doc_id_ < doc_id);
    RowID target_doc_id = doc_id;
    bool breaked = false;
    do {
        bool ok = inner_iterators_[0]->Next(doc_id);
        if (!ok){
            return false;
        }
        target_doc_id = inner_iterators_[0]->DocID();
        for (u32 i = 1; i < inner_iterators_.size(); ++i) {
            ok = inner_iterators_[i]->Next(target_doc_id);
            if(ok && target_doc_id == inner_iterators_[i]->DocID()){
                breaked = true;
                break;
            }
        }
        if (!breaked &&inner_iterators_[0]->BM25Score() < threshold_){
            breaked = true;
        }
    } while(breaked);
    doc_id_ = target_doc_id;
    return true;
}

Pair<bool, RowID> BlockMaxAndNotIterator::SeekInBlockRange(RowID doc_id, const RowID doc_id_no_beyond) {
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        if (doc_id > block_end) [[unlikely]] {
            return {false, INVALID_ROWID};
        }
        const auto [success, id] = inner_iterators_[0]->SeekInBlockRange(doc_id, block_end);
        if (!success) {
            return {false, INVALID_ROWID};
        }
        assert((doc_id <= id && id <= block_end));
        doc_id = id;
        bool all_match = true;
        for (u32 i = 1; i < inner_iterators_.size(); ++i) {
            if (inner_iterators_[i]->NotPartCheckExist(doc_id)) {
                all_match = false;
                break;
            }
        }
        if (all_match) {
            doc_id_ = doc_id;
            return {true, doc_id};
        }
        ++doc_id;
    }
}

Tuple<bool, float, RowID> BlockMaxAndNotIterator::SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) {
    if (threshold > BlockMaxBM25Score()) [[unlikely]] {
        return {false, 0.0F, INVALID_ROWID};
    }
    const RowID block_end = std::min(doc_id_no_beyond, BlockLastDocID());
    while (true) {
        if (doc_id > block_end) [[unlikely]] {
            return {false, 0.0F, INVALID_ROWID};
        }
        auto [success, score, id] = inner_iterators_[0]->SeekInBlockRange(doc_id, block_end, threshold);
        if (!success) {
            return {false, 0.0F, INVALID_ROWID};
        }
        assert((doc_id <= id && id <= block_end));
        doc_id = id;
        bool all_match = true;
        for (u32 i = 1; i < inner_iterators_.size(); ++i) {
            if (inner_iterators_[i]->NotPartCheckExist(doc_id)) {
                all_match = false;
                break;
            }
        }
        if (all_match) {
            doc_id_ = doc_id;
            return {true, score, doc_id};
        }
        ++doc_id;
    }
}

Pair<bool, RowID> BlockMaxAndNotIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    return inner_iterators_[0]->PeekInBlockRange(doc_id, doc_id_no_beyond);
}

bool BlockMaxAndNotIterator::NotPartCheckExist(RowID doc_id) {
    if (doc_id_ > doc_id) {
        return false;
    }
    if (doc_id_ == doc_id) {
        return true;
    }
    if (!inner_iterators_[0]->NotPartCheckExist(doc_id)) {
        return false;
    }
    for (u32 i = 1; i < inner_iterators_.size(); ++i) {
        if (inner_iterators_[i]->NotPartCheckExist(doc_id)) {
            return false;
        }
    }
    doc_id_ = doc_id;
    return true;
}

} // namespace infinity