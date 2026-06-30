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

export module infinity_core:block_at_a_time_iterator;

import :index_defines;
import :doc_iterator;
import :blockmax_leaf_iterator;
import :term_doc_iterator;
import :multi_doc_iterator;
import :simd_functions;

import std.compat;
import internal_types;

namespace infinity {

// Block-at-a-time (BAAT) iterator for long queries with many common terms.
//
// Processing order: terms sorted by IDF descending (rare first).
// - Rare terms → few docs, small accumulator → quickly establish top-k threshold.
// - Common terms → long posting lists → block-level pruning using threshold.
//
// For TermDocIterator children, per-block SIMD BM25 batch computation is used
// (reusing GetSIMD_FUNCTIONS().BatchBM25_func_ptr_) to compute all document
// scores in a block with a single AVX2-vectorized call.
//
// A running min-heap maintains the current top-k threshold during accumulation,
// updated after each term so subsequent terms can skip irrelevant blocks.
export class BlockAtATimeIterator : public MultiDocIterator {
public:
    explicit BlockAtATimeIterator(std::vector<std::unique_ptr<DocIterator>> &&iterators, u32 topn);

    ~BlockAtATimeIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kBlockAtATimeIterator; }

    std::string Name() const override { return "BlockAtATimeIterator"; }

    void UpdateScoreThreshold(float threshold) override;

    bool Next(RowID doc_id) override;

    float BM25Score() const;

    float Score() override { return BM25Score(); }

    u32 MatchCount() const override;

    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;

private:
    // Per-term BM25 parameters (extracted from the child iterator)
    struct TermBM25Params {
        BlockMaxLeafIterator *leaf_iter = nullptr;
        bool is_term_doc = false; // true → TermDocIterator, supports SIMD batch
        float f1 = 0.0f;
        float f2 = 0.0f;
        float bm25_common_score = 0.0f;
        float score_upper_bound = 0.0f;
    };

    // Accumulator entry for a single document
    struct AccumEntry {
        float score = 0.0f;
        u32 match_count = 0;
    };

    // Min-heap comparator by score (used for running top-k)
    struct MinScore {
        bool operator()(const std::pair<RowID, float> &a, const std::pair<RowID, float> &b) const { return a.second > b.second; }
    };

    void SortTermsByIDF();
    void ProcessTerm(u32 term_idx);
    void ProcessBlockSIMD(TermDocIterator *tdi, RowID block_min, RowID block_last, u32 term_idx);
    void AllocateBatchBuffers();
    void AccumulateDoc(RowID doc_id, float score, u32 match);
    void UpdateThresholdFromAccumulator();
    void ExtractTopK();

    u32 topn_;

    std::vector<TermBM25Params> term_params_;
    std::vector<u32> sorted_term_order_;

    // Sparse accumulator: doc_id → (score, match_count)
    std::unordered_map<RowID, AccumEntry> accumulator_;

    // Running min-heap of top-k candidates; updated incrementally during accumulation
    std::priority_queue<std::pair<RowID, float>, std::vector<std::pair<RowID, float>>, MinScore> running_topk_heap_;

    // Final sorted results
    std::vector<std::pair<RowID, float>> topk_results_;
    size_t topk_cursor_ = 0;

    mutable float bm25_score_cache_ = 0.0f;
    bool score_cached_ = false;

    // SIMD batch buffers (64-byte aligned, BAAT_BATCH_SIZE entries each)
    void *aligned_buffer_ = nullptr;
    u32 *batch_tf_ = nullptr;
    u32 *batch_doc_len_ = nullptr;
    u32 *batch_match_cnt_ = nullptr;
    f32 *batch_score_sum_ = nullptr;

    u64 total_docs_processed_ = 0;
    u64 total_blocks_skipped_ = 0;
    u64 total_blocks_decoded_ = 0;
};

} // namespace infinity
