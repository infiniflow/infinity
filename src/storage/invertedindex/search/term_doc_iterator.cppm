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

export module term_doc_iterator;

import stl;
import memory_pool;
import posting_iterator;
import bitmap;
import index_defines;
import term_meta;
import doc_iterator;
import match_data;
import internal_types;

namespace infinity {
export class TermDocIterator : public DocIterator {
public:
    TermDocIterator(PostingIterator *iter, u64 column_id);

    virtual ~TermDocIterator();

    void AddIterator(DocIterator *iter) override {}

    void DoSeek(RowID doc_id) override;

    u32 GetDF() const override { return iter_->GetTermMeta()->GetDocFreq(); }

    bool GetTermMatchData(TermColumnMatchData &match_data, RowID doc_id) override {
        if (doc_id == doc_id_) {
            match_data.doc_id_ = doc_id_;
            iter_->GetTermMatchData(match_data);
            return true;
        }
        return false;
    }

private:
    u64 column_id_;
    PostingIterator *iter_ = nullptr;
};
} // namespace infinity
