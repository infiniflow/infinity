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

import posting_iterator;
import index_defines;
import term_meta;
import doc_iterator;
import match_data;
import internal_types;
import doc_iterator;
import third_party;

namespace infinity {
export class TermDocIterator final : public DocIterator {
public:
    TermDocIterator(UniquePtr<PostingIterator> &&iter, u64 column_id, float weight)
        : column_id_(column_id), iter_(std::move(iter)), weight_(weight) {
        doc_freq_ = iter_->GetDocFreq();
        term_freq_ = 0;
    }

    void DoSeek(RowID doc_id) override { doc_id_ = iter_->SeekDoc(doc_id); }

    u32 GetDF() const override { return doc_freq_; }

    bool GetTermMatchData(TermColumnMatchData &match_data, RowID doc_id) {
        if (doc_id == doc_id_) {
            match_data.doc_id_ = doc_id_;
            iter_->GetTermMatchData(match_data);
            term_freq_ += match_data.tf_;
            return true;
        }
        return false;
    }

    float GetWeight() const { return weight_; }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    DocIteratorType GetType() const override { return DocIteratorType::kTermIterator; }

    u64 GetTermFreq() const { return term_freq_; }

    // debug info
    const String *term_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    u64 column_id_;
    UniquePtr<PostingIterator> iter_;
    u32 doc_freq_;
    float weight_;
    u64 term_freq_;
};
} // namespace infinity
