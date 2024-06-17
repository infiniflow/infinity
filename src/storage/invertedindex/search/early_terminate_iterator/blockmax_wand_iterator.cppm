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

export module blockmax_wand_iterator;
import stl;
import index_defines;
import early_terminate_iterator;
import internal_types;

namespace infinity {

// Refers to https://engineering.nyu.edu/~suel/papers/bmw.pdf
export class BlockMaxWandIterator final : public EarlyTerminateIterator {
public:
    explicit BlockMaxWandIterator(Vector<UniquePtr<EarlyTerminateIterator>> iterators);

    ~BlockMaxWandIterator() override;

    void UpdateScoreThreshold(float threshold) override;

    bool NextShallow(RowID doc_id) override;

    bool Next(RowID doc_id) override;

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
        return MultiQueryEarlyTerminateIteratorCommonPrintTree(this, "BlockMaxWandIterator", sorted_iterators_, os, prefix, is_final);
    }

private:
    // block max info
    RowID common_block_min_possible_doc_id_{}; // not always exist
    RowID common_block_last_doc_id_{};
    float common_block_max_bm25_score_{};
    Vector<UniquePtr<EarlyTerminateIterator>> sorted_iterators_; // sort by DocID(), in ascending order
    Vector<UniquePtr<EarlyTerminateIterator>> backup_iterators_;
    SizeT pivot_;
    // bm25 score cache
    bool bm25_score_cached_ = false;
    float bm25_score_cache_ = 0.0f;
    Vector<Tuple<u32, u64, float>> pivot_history_; //pivot, row_id, score
    // debug info
    u32 next_sort_cnt_ = 0;
    u32 next_it0_docid_mismatch_cnt_ = 0;
    u32 next_sum_score_low_cnt_ = 0;
    u32 next_sum_score_bm_low_cnt_ = 0;
    Vector<SizeT> next_sum_score_bm_low_cnt_dist_{};
};

} // namespace infinity