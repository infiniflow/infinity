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
import doc_iterator;
import term_doc_iterator;
import multi_doc_iterator;
import internal_types;
import index_defines;

namespace infinity {

// equivalent to "OR" iterator
export class BlockMaxMaxscoreIterator final : public MultiDocIterator {
public:
    explicit BlockMaxMaxscoreIterator(Vector<UniquePtr<DocIterator>> &&iterators);

    ~BlockMaxMaxscoreIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kBMMIterator; }

    String Name() const override { return "BlockMaxMaxscoreIterator"; }

    void UpdateScoreThreshold(float threshold) override;

    bool Next(RowID doc_id) override;

    float BM25Score() override;

    u32 LeafCount() const override;

    u32 MatchCount() const override;

private:
    void Init();
    // won't change after initialization
    Vector<TermDocIterator *> sorted_iterators_;                 // sort by BM25ScoreUpperBound, in descending order
    Vector<float> leftover_scores_upper_bound_;                  // value at i: upper bound of sum of BM25 scores for iter i + 1, i + 2, ..., n - 1
    // block max info
    RowID common_block_min_possible_doc_id_ = INVALID_ROWID;
    RowID common_block_last_doc_id_ = INVALID_ROWID;
    float common_block_max_bm25_score_ = 0.0f;
    Vector<float> common_block_max_bm25_score_parts_; // value at i: blockmax of sum of BM25 scores for iter i + 1, i + 2, ..., n - 1
    // pivot and must_have info
    // separate the iterators into two parts:
    // 1. [0, pivot) : cannot add into part 2
    // 2. [pivot, n) : sum < threshold
    u32 pivot_ = sorted_iterators_.size();
    // there is an "AND" requirement for the iterators 0, 1, ..., must_have_before_ - 1
    // we have must_have_before_ > 0 <=> pivot_ <= 1.
    // so: case 1: must_have_before_ = 0, pivot_ > 1.
    //     case 2: must_have_before_ > 0, pivot_ = 1.
    u32 must_have_before_ = 0;
    float must_have_total_upper_bound_score_ = 0.0f;
    Vector<int> bool_need_score_;
    // bm25 score cache
    bool bm25_score_cached_ = false;
    bool need_seek_after_must_ = false;
    bool need_seek_after_pivot_ = false;
    float bm25_score_cache_ = 0.0f;
    RowID prev_next_candidate_ = INVALID_ROWID;

    // debug info
    u32 inner_pivot_loop_cnt = 0;
    u32 inner_must_have_loop_cnt_ = 0;
    u32 use_prev_candidate_cnt_ = 0;
    u32 not_use_prev_candidate_cnt_ = 0;
    Vector<Pair<u32, u64>> pivot_history_;
    Vector<Pair<u32, u64>> must_have_history_;
};

} // namespace infinity