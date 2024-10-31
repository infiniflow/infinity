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

#include <iostream>

module score_threshold_iterator;
import stl;
import doc_iterator;
import internal_types;
import index_defines;

namespace infinity {

bool ScoreThresholdIterator::Next(RowID doc_id) {
    while (true) {
        if (!(query_iterator_->Next(doc_id))) {
            doc_id_ = INVALID_ROWID;
            return false;
        }
        doc_id = query_iterator_->DocID();
        // check score
        if (BM25Score() >= score_threshold_) {
            doc_id_ = doc_id;
            return true;
        }
        ++doc_id;
    }
}

void ScoreThresholdIterator::PrintTree(std::ostream &os, const String &prefix, const bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "ScoreThresholdIterator (score_threshold: " << score_threshold_ << ")\n";
    const String next_prefix = prefix + (is_final ? "    " : "│   ");
    query_iterator_->PrintTree(os, next_prefix, true);
}

} // namespace infinity
