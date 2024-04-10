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

export module blockmax_term_doc_iterator;

import stl;
import index_defines;
import internal_types;
import posting_iterator;
import term_doc_iterator;
import early_terminate_iterator;

namespace infinity {
class MemoryPool;
class SegmentPosting;
class FullTextColumnLengthReader;

export class BlockMaxTermDocIterator final : public EarlyTerminateIterator {
public:
    BlockMaxTermDocIterator(optionflag_t flag, MemoryPool *session_pool);

    bool InitPostingIterator(SharedPtr<Vector<SegmentPosting>> seg_postings, const u32 state_pool_size);

    void MultiplyWeight(float factor) { weight_ *= factor; }

    void UpdateScoreThreshold(float threshold) override {} // do nothing

    bool BlockSkipTo(RowID doc_id, float threshold) override;

    // u32: block max tf
    // u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
    Pair<u32, u16> GetBlockMaxInfo() const { return iter_.GetBlockMaxInfo(); }

    void InitBM25Info(u64 total_df, float avg_column_len, FullTextColumnLengthReader *column_length_reader);

    RowID BlockMinPossibleDocID() const override { return iter_.BlockLowestPossibleDocID(); }

    RowID BlockLastDocID() const override { return iter_.BlockLastDocID(); }

    // weight included
    float BlockMaxBM25Score() override;

    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) override;

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;

    bool Seek(RowID doc_id) override;

    // weight included
    float BM25Score();

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

    // debug info
    const String *term_ptr_ = nullptr;
    const String *column_name_ptr_ = nullptr;

private:
    // similar to TermDocIterator
    PostingIterator iter_; // initialized in constructor and InitPostingIterator() function
    float weight_ = 1.0f;  // changed in MultiplyWeight()
    // for BM25 Score
    float avg_column_len_ = 0;
    FullTextColumnLengthReader *column_length_reader_ = nullptr;
    float bm25_common_score_ = 0; // include: weight * smooth_idf * (k1 + 1.0F)
    float block_max_bm25_score_cache_ = 0;
    RowID block_max_bm25_score_cache_end_id_ = INVALID_ROWID;
    // cache for PeekInBlockRange
    RowID peek_doc_id_range_start_ = INVALID_ROWID;
    RowID peek_doc_id_range_end_ = INVALID_ROWID;
    RowID peek_doc_id_val_ = INVALID_ROWID;
};

} // namespace infinity
