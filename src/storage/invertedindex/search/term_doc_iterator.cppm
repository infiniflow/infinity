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
import internal_types;
import doc_iterator;
import column_length_io;
import third_party;

namespace infinity {
export class TermDocIterator final : public DocIterator {
public:
    TermDocIterator(UniquePtr<PostingIterator> &&iter, u64 column_id, float weight);

    ~TermDocIterator() override;

    float GetWeight() const { return weight_; }

    void MultiplyWeight(float factor) { weight_ *= factor; }

    inline u32 GetDocFreq() const { return doc_freq_; }

    u64 GetTermFreq() const { return term_freq_; }

    void InitBM25Info(UniquePtr<FullTextColumnLengthReader> &&column_length_reader);

    RowID BlockMinPossibleDocID() const { return iter_->BlockLowestPossibleDocID(); }
    RowID BlockLastDocID() const { return iter_->BlockLastDocID(); }
    float BlockMaxBM25Score();

    // Move block cursor to ensure its last_doc_id is no less than given doc_id.
    // Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
    // Note that this routine decode skip_list only, and doesn't update doc_id_ when returns true.
    // Caller may invoke BlockMaxBM25Score() after this routine.
    bool NextShallow(RowID doc_id);

    // Overriden methods
    DocIteratorType GetType() const override { return DocIteratorType::kTermDocIterator; }

    String Name() const override { return "TermDocIterator"; }

    bool Next(RowID doc_id) override;

    float BM25Score() override;

    void UpdateScoreThreshold(float threshold) override {
        if (threshold > threshold_)
            threshold_ = threshold;
    }

    u32 MatchCount() const override { return DocID() != INVALID_ROWID; }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    // debug info
    const String *term_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    Pair<tf_t, u32> GetScoreData();

    u32 doc_freq_ = 0;

    u64 column_id_;
    UniquePtr<PostingIterator> iter_;
    float weight_ = 1.0f; // changed in MultiplyWeight()
    u64 term_freq_;

    // for BM25 Score
    float bm25_score_cache_ = 0.0f;
    RowID bm25_score_cache_docid_ = INVALID_ROWID;
    float f1 = 0.0f;
    float f2 = 0.0f;
    float f3 = 0.0f;
    float avg_column_len_ = 0;
    UniquePtr<FullTextColumnLengthReader> column_length_reader_ = nullptr;
    float bm25_common_score_ = 0; // include: weight * smooth_idf * (k1 + 1.0F)
    float block_max_bm25_score_cache_ = 0.0f;
    RowID block_max_bm25_score_cache_end_id_ = INVALID_ROWID;

    // debug info
    u32 calc_score_cnt_ = 0;
    u32 access_score_cnt_ = 0;
    u32 calc_bm_score_cnt_ = 0;
    u32 access_bm_score_cnt_ = 0;
    u32 duplicate_calc_score_cnt_ = 0;
    u32 seek_cnt_ = 0;
    u32 peek_cnt_ = 0;
    u32 block_skip_cnt_ = 0;
    u32 block_skip_cnt_inner_ = 0;
};
} // namespace infinity
