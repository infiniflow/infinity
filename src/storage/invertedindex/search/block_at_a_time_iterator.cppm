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
import :multi_doc_iterator;
import :simd_functions;

import std.compat;
import internal_types;

namespace infinity {

// Block-at-a-time (BAAT) iterator for long queries with many common terms.
// Processes one term's posting list at a time, accumulating scores in a sparse
// accumulator, then extracts top-k results.
//
// Reference: Hornet engine BAAT traversal for agent workloads (long queries).
// BAAT is selected when query has >= 8 term children and average DF > 10% of total docs.
// In BAAT, terms are processed in descending IDF order (rare terms first) to
// quickly establish a high threshold, enabling aggressive block-level pruning
// for subsequent common terms.
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

private:
    // Per-term BM25 parameters (extracted from TermDocIterator)
    struct TermBM25Params {
        BlockMaxLeafIterator *leaf_iter = nullptr;
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

    // Sort term indices by IDF descending (rare terms first)
    void SortTermsByIDF();

    // Process a single term's entire posting list, accumulating scores
    void ProcessTerm(u32 term_idx);

    // Extract top-k results from the accumulator into a sorted list
    void ExtractTopK();

    // Top-k
    u32 topn_;

    // Term BM25 parameters (pointer into children_)
    std::vector<TermBM25Params> term_params_;

    // Term order sorted by IDF descending
    std::vector<u32> sorted_term_order_;

    // Sparse accumulator: doc_id -> (score, match_count)
    std::unordered_map<RowID, AccumEntry> accumulator_;

    // Final top-k results (doc_id, score) sorted by score descending
    std::vector<std::pair<RowID, float>> topk_results_;

    // Current iteration cursor into topk_results_
    size_t topk_cursor_ = 0;

    // BM25 score cache for current doc
    mutable float bm25_score_cache_ = 0.0f;
    bool score_cached_ = false;

    // Debug stats
    u64 total_docs_processed_ = 0;
    u64 total_blocks_skipped_ = 0;
    u64 total_blocks_decoded_ = 0;
};

} // namespace infinity
