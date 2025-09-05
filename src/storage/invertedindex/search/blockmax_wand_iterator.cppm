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

export module infinity_core:blockmax_wand_iterator;

import :index_defines;
import :doc_iterator;
import :blockmax_leaf_iterator;
import :multi_doc_iterator;

import internal_types;

namespace infinity {

// Refers to https://engineering.nyu.edu/~suel/papers/bmw.pdf
export class BlockMaxWandIterator : public MultiDocIterator {
public:
    explicit BlockMaxWandIterator(std::vector<std::unique_ptr<DocIterator>> &&iterators);

    ~BlockMaxWandIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kBMWIterator; }

    std::string Name() const override { return "BlockMaxWandIterator"; }

    void UpdateScoreThreshold(float threshold) override;

    bool Next(RowID doc_id) override;

    float BM25Score();

    float Score() override { return BM25Score(); }

    u32 MatchCount() const override;

private:
    // Advanced optimization methods
    size_t FindPivotOptimized(float threshold);
    void UpdateScoreUpperBoundPrefixSums();
    bool ShouldSkipSort() const;
    void OptimizedPartialSort(size_t limit);
    bool TryFastPivotEstimation(float threshold, size_t& estimated_pivot);

    // block max info
    RowID common_block_min_possible_doc_id_{}; // not always exist
    RowID common_block_last_doc_id_{};
    float common_block_max_bm25_score_{};
    std::vector<BlockMaxLeafIterator *> sorted_iterators_; // sort by DocID(), in ascending order
    std::vector<BlockMaxLeafIterator *> backup_iterators_;
    size_t pivot_;

    // Enhanced optimization for many keywords
    static constexpr u32 SORT_SKIP_THRESHOLD = 15; // Reduced threshold for better balance
    static constexpr u32 LAZY_SORT_INTERVAL = 3;   // More frequent sorting for accuracy
    static constexpr u32 FAST_PIVOT_THRESHOLD = 50; // Use fast estimation for very large sets
    static constexpr u32 PARTIAL_SORT_FACTOR = 3;   // Sort only top 1/3 for large sets
    
    std::vector<f32> score_ub_prefix_sums_;        // Prefix sums for fast pivot calculation
    std::vector<size_t> iterator_indices_;         // Cached indices for avoiding pointer chasing
    bool prefix_sums_valid_ = false;
    bool indices_valid_ = false;
    u32 iterations_since_sort_ = 0;
    u32 consecutive_skips_ = 0;                    // Track consecutive sort skips

    // bm25 score cache
    bool bm25_score_cached_ = false;
    float bm25_score_cache_ = 0.0f;
    std::vector<std::tuple<u32, u64, float>> pivot_history_; // pivot, row_id, score
    // debug info
    u32 next_sort_cnt_ = 0;
    u32 next_it0_docid_mismatch_cnt_ = 0;
    u32 next_sum_score_low_cnt_ = 0;
    u32 next_sum_score_bm_low_cnt_ = 0;
    std::vector<size_t> next_sum_score_bm_low_cnt_dist_{};
};

} // namespace infinity
