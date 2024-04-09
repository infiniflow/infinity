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

export module blockmax_maxscore_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;

namespace infinity {

// equivalent to "OR" iterator
export class BlockMaxMaxscoreIterator final : public EarlyTerminateIterator {
public:
    explicit BlockMaxMaxscoreIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators);

    Pair<RowID, float> NextWithThreshold(float threshold) override;

    Pair<RowID, float> BlockNextWithThreshold(float threshold) override;

    void UpdateScoreThreshold(float threshold) override;

    bool BlockSkipTo(RowID doc_id, float threshold) override;

    // following functions are available only after BlockSkipTo() is called

    RowID BlockMinPossibleDocID() const override { return common_block_min_possible_doc_id_; }

    RowID BlockLastDocID() const override { return common_block_last_doc_id_; }

    float BlockMaxBM25Score() override { return common_block_max_bm25_score_; }

    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, float threshold, RowID doc_id_no_beyond) override;

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;

private:
    // block max info
    u32 pivot = 0;                                           // seperate the iterators into two parts: [0, pivot) and [pivot, n)
    RowID common_block_min_possible_doc_id_ = INVALID_ROWID; // not always exist
    RowID common_block_last_doc_id_ = INVALID_ROWID;
    float common_block_max_bm25_score_ = 0.0f;
    Vector<float> common_block_max_bm25_score_until_; // value at i: blockmax of sum of BM25 scores for iter 0, 1, ..., i
    // won't change after initialization
    Vector<float> bm25_scores_upper_bound_until_;                // value at i: upper bound of sum of BM25 scores for iter 0, 1, ..., i
    Vector<UniquePtr<EarlyTerminateIterator>> sorted_iterators_; // sort by BM25ScoreUpperBound, in ascending order
};

} // namespace infinity