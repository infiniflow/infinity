// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
#include <iostream>
module rank_feature_doc_iterator;

import stl;
import logger;

namespace infinity {

RankFeatureDocIterator::RankFeatureDocIterator(UniquePtr<PostingIterator> &&iter, const u64 column_id, float boost)
    : column_id_(column_id), boost_(boost), iter_(std::move(iter)) {}

RankFeatureDocIterator::~RankFeatureDocIterator() {}

bool RankFeatureDocIterator::Next(RowID doc_id) {
    assert(doc_id != INVALID_ROWID);
    if (doc_id_ != INVALID_ROWID && doc_id_ >= doc_id)
        return true;
    doc_id_ = iter_->SeekDoc(doc_id);
    if (doc_id_ == INVALID_ROWID)
        return false;
    return true;
}

float RankFeatureDocIterator::Score() {
    u16 payload = iter_->GetCurrentDocPayload();
    float weight = static_cast<float>(payload);
    return weight * boost_;
}

void RankFeatureDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "RankFeatureDocIterator";
    os << " (column: " << *column_name_ptr_ << ")";
    os << " (term: " << *term_ptr_ << ")";
    os << '\n';
}

} // namespace infinity
