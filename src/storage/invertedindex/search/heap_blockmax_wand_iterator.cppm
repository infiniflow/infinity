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

export module infinity_core:heap_blockmax_wand_iterator;

import :doc_iterator;
import :multi_doc_iterator;
import :blockmax_leaf_iterator;

import internal_types;

namespace infinity {

// Block-max WAND over a doc-ordered binary heap.
//
// The only difference from the sort-based BlockMaxWandIterator is how the doc-ordered prefix is
// obtained:
//   - sort-based: re-sorts all n sub-iterators every pivot round and relies on the implicit
//     invariant that the whole array stays ordered;
//   - this implementation: keeps a doc-ordered min-heap and pops only the part needed to make the
//     upper-bound prefix sum exceed the threshold into a scratch buffer (prefix_), then pushes it
//     back. prefix_ is scratch state, so reordering it (e.g. by BlockLastDocID to find the
//     earliest block end) cannot break any invariant.
//
// It also fixes two correctness hazards of the sort-based version:
//   1) never call NextShallow on a sub-iterator already positioned on the candidate doc d.
//      NextShallow only moves the block cursor without updating doc_id_, so the following
//      BM25Score() reads tf=0 (a non-zero mis-score for common*f4) or a stale cached score from
//      the previous doc. This implementation only calls NextShallow for DocID() < d.
//   2) for a sub-iterator whose block was skipped past d, its contribution at d is 0 rather than
//      the upper bound of the block it landed in. The latter over-estimates d and lets a
//      non-competitive doc into the scoring path, producing a wrong score.
//      (0 is provable: the child skipped because its block bound <= local_threshold =
//       thr - sum_{j!=i} ub_j, hence d's score <= thr and d must not be returned.)
// Iteration statistics. Used by the integration tests to assert that pruning really happens, and
// useful for diagnosing long queries in production.
export struct BlockMaxWandStats {
    u64 iterations_ = 0;     // pivot rounds (outer loop iterations)
    u64 docs_scored_ = 0;    // candidates that went through exact scoring
    u64 child_advances_ = 0; // child advances via Next (decodes tf)
    u64 block_seeks_ = 0;    // child block-level seeks via NextShallow (no tf decoding)
    u64 heap_ops_ = 0;       // heap push/pop count (control-structure overhead)
    u64 retired_children_ = 0;
};

// Not final: MinimumShouldMatchWrapper<T> derives from T, so this class must stay derivable
// (same as BlockMaxWandIterator).
export class HeapBlockMaxWandIterator : public MultiDocIterator {
public:
    // How children are advanced (A/B switch):
    //   kThresholdSkip -- BlockMaxLeafIterator::Next(); the leaf skips docs by its own local
    //                     threshold (decodes tf for every candidate)
    //   kPlainSeek     -- NextNoSkip(); positioning only, competitiveness is decided here by the
    //                     block upper bounds
    enum class AdvanceMode : u8 {
        kThresholdSkip,
        kPlainSeek,
    };

    static void SetAdvanceMode(AdvanceMode mode);
    static AdvanceMode GetAdvanceMode();

    explicit HeapBlockMaxWandIterator(std::vector<std::unique_ptr<DocIterator>> &&children);

    ~HeapBlockMaxWandIterator() override = default;

    DocIteratorType GetType() const override { return DocIteratorType::kHeapBMWIterator; }

    std::string Name() const override { return "HeapBlockMaxWandIterator"; }

    bool Next(RowID doc_id) override;

    float Score() override;

    void UpdateScoreThreshold(float threshold) override;

    u32 MatchCount() const override;

    void SetMinimumShouldMatchHint(u32 minimum_should_match) override;

    void PrintTree(std::ostream &os, const std::string &prefix, bool is_final) const override;

    const BlockMaxWandStats &GetStats() const { return stats_; }

private:
    // Initializes: advance every child to its first doc and build the heap.
    void Init();

    // Terminates: clears the state and returns false.
    bool Finish();

    // Advances idx to >= target and pushes it back on success; retires it when the child is exhausted.
    void PushBack(u32 idx, RowID target);

    void Retire(u32 idx);

    void RestorePrefix();

    // Advances idx in prefix_ to >= target, retiring it when exhausted. It must not be pushed back
    // directly, otherwise INVALID_ROWID would take part in pivot computation as a huge doc.
    void AdvanceWithin(u32 idx, RowID target);

    // Called after from_doc was proven non-competitive: uses NextShallow (skip-list decode only,
    // no tf decode) to move the prefix children forward block by block until the range upper bound
    // exceeds the threshold or up_to (heap top) is reached. Before returning it guarantees that
    // every non-retired prefix child has a real doc_id_ (positioned via Next), so the prefix can be
    // pushed back on the heap safely.
    void SkipUncompetitiveRange(RowID from_doc);

    // Degraded variant of SkipUncompetitiveRange: with block skipping disabled it just advances one
    // doc per prefix child. Used to tell "block skip caused a recall miss" apart from other causes
    // (environment variable INFINITY_BMW_IT_NOSKIP).
    void SkipUncompetitiveRangeOrAdvanceOne(RowID from_doc);

    u32 PopHead();

    void PushHead(u32 idx);

    // std::*_heap comparator: true when lhs has lower priority (larger doc).
    bool HeadLess(u32 lhs, u32 rhs) const { return ctx_[lhs]->DocID() > ctx_[rhs]->DocID(); }

    std::vector<BlockMaxLeafIterator *> ctx_;
    // Min-heap over indices into ctx_, ordered by DocID().
    std::vector<u32> head_;
    // Doc-ordered prefix popped from head_; whenever Next() returns true it is also the "lead"
    // positioned on doc_id_.
    std::vector<u32> prefix_;
    void AdvanceChild(u32 idx, RowID target);

    std::vector<u8> retired_;
    BlockMaxWandStats stats_{};
    float score_cache_ = 0.0f;
    bool score_cached_ = false;
    u32 minimum_should_match_hint_ = 0;
};

} // namespace infinity
