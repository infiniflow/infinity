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

export module blockmax_and_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;

namespace infinity {

export class BlockMaxAndIterator final : public EarlyTerminateIterator {
public:
    explicit BlockMaxAndIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators);

    void UpdateScoreThreshold(float threshold) override;

    bool BlockSkipTo(RowID doc_id, float threshold) override;

    // following functions are available only after BlockSkipTo() is called

    RowID BlockMinPossibleDocID() const override { return common_block_min_possible_doc_id_; }

    RowID BlockLastDocID() const override { return common_block_last_doc_id_; }

    float BlockMaxBM25Score() override { return common_block_max_bm25_score_; }

    Pair<bool, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;
    Tuple<bool, float, RowID> SeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond, float threshold) override;
    float BM25Score() override;

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) override;

    bool NotPartCheckExist(RowID doc_id) override;

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override {
        return MultiQueryEarlyTerminateIteratorCommonPrintTree(this, "BlockMaxAndIterator", sorted_iterators_, os, prefix, is_final);
    }

private:
    // block max info
    RowID common_block_min_possible_doc_id_{}; // not always exist
    RowID common_block_last_doc_id_{};
    float common_block_max_bm25_score_{};
    Vector<float> common_block_max_bm25_score_parts_; // value at i: blockmax of sum of BM25 scores for iter i + 1, i + 2, ..., n - 1
    // won't change after initialization
    Vector<float> leftover_scores_upper_bound_;                  // value at i: upper bound of sum of BM25 scores for iter i + 1, i + 2, ..., n - 1
    Vector<UniquePtr<EarlyTerminateIterator>> sorted_iterators_; // sort by df, in ascending order
    // bm25 score cache
    bool bm25_score_cached_ = false;
    float bm25_score_cache_ = 0.0F;
};

} // namespace infinity