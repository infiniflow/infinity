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

module and_not_iterator;

import stl;
import index_defines;
import multi_doc_iterator;
import doc_iterator;
import internal_types;

namespace infinity {

AndNotIterator::AndNotIterator(Vector<UniquePtr<DocIterator>> iterators) : MultiDocIterator(std::move(iterators)) {
    std::sort(children_.begin() + 1, children_.end(), [](const auto &lhs, const auto &rhs) { return lhs->GetDF() < rhs->GetDF(); });
    // initialize doc_id_ to first doc
    Next(0);
    // init df
    doc_freq_ = children_[0]->GetDF();
    bm25_score_upper_bound_ = children_[0]->BM25ScoreUpperBound();
}

bool AndNotIterator::Next(RowID doc_id) {
    bool next_loop = false;
    do {
        children_[0]->Next(doc_id);
        doc_id = children_[0]->DocID();
        if (doc_id == INVALID_ROWID) [[unlikely]] {
            break;
        }
        // now doc_id < INVALID_ROWID
        next_loop = false;
        for (u32 i = 1; i < children_.size(); ++i) {
            children_[i]->Next(doc_id);
            if (RowID doc = children_[i]->DocID(); doc == doc_id) {
                ++doc_id;
                next_loop = true;
                break;
            }
        }
    } while (next_loop);
    doc_id_ = doc_id;
    return doc_id != INVALID_ROWID;
}

float AndNotIterator::BM25Score() { return children_[0]->BM25Score(); }

void AndNotIterator::UpdateScoreThreshold(float threshold) { children_[0]->UpdateScoreThreshold(threshold); }

u32 AndNotIterator::MatchCount() const { return children_[0]->MatchCount(); }

} // namespace infinity