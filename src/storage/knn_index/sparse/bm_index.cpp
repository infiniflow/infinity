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

#include <vector>
#include <xmmintrin.h>

module bm_index;

import infinity_exception;
import third_party;
import knn_result_handler;
import serialize;
import bm_simd_func;

namespace infinity {

template <bool UseSIMD>
void PostingList::Calculate(Vector<f32> &upper_bounds, f32 query_score) const {
    if constexpr (UseSIMD) {
        MultiF32StoreI32(block_ids_.data(), max_scores_.data(), upper_bounds.data(), query_score, block_ids_.size());
    } else {
        for (SizeT i = 0; i < block_ids_.size(); ++i) {
            i32 block_id = block_ids_[i];
            f32 score = max_scores_[i];
            upper_bounds[block_id] += score * query_score;
        }
    }
}

template void PostingList::Calculate<false>(Vector<f32> &upper_bounds, f32 query_score) const;
template void PostingList::Calculate<true>(Vector<f32> &upper_bounds, f32 query_score) const;

void BMIvt::AddBlock(i32 block_id, const Vector<Vector<Pair<i32, f32>>> &tail_terms) {
    HashMap<i32, f32> max_scores;
    for (const auto &terms : tail_terms) {
        for (const auto &[term_id, score] : terms) {
            max_scores[term_id] = std::max(max_scores[term_id], score);
        }
    }
    for (const auto &[term_id, score] : max_scores) {
        postings_[term_id].block_ids_.push_back(block_id);
        postings_[term_id].max_scores_.push_back(score);
    }
}

void BMIvt::Optimize(i32 topk, Vector<Vector<f32>> ivt_scores) {
    for (i32 term_id = 0; term_id < (i32)ivt_scores.size(); ++term_id) {
        auto &posting = postings_[term_id];
        auto &term_scores = ivt_scores[term_id];
        posting.kth_ = topk;
        if ((i32)term_scores.size() < topk) {
            continue;
        }
        std::partial_sort(term_scores.begin(), term_scores.begin() + topk - 1, term_scores.end(), std::greater<>());
        posting.kth_score_ = term_scores[topk - 1];
    }
}

i8 TailFwd::AddDoc(const SparseVecRef<f32, i32> &doc) {
    Vector<Pair<i32, f32>> doc_terms;
    for (i32 i = 0; i < doc.nnz_; ++i) {
        doc_terms.emplace_back(doc.indices_[i], doc.data_[i]);
    }
    tail_terms_.emplace_back(std::move(doc_terms));
    return tail_terms_.size();
}

Vector<Tuple<i32, Vector<i8>, Vector<f32>>> TailFwd::ToBlockFwd() const {
    Vector<Tuple<i32, i8, f32>> term_pairs;
    i8 block_size = tail_terms_.size();
    for (i8 block_offset = 0; block_offset < block_size; ++block_offset) {
        for (const auto &[term_id, score] : tail_terms_[block_offset]) {
            term_pairs.emplace_back(term_id, block_offset, score);
        }
    }
    std::sort(term_pairs.begin(), term_pairs.end(), [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });

    Vector<Tuple<i32, Vector<i8>, Vector<f32>>> res;
    i32 last_term_id = -1;
    Vector<i8> block_offsets;
    Vector<f32> scores;
    for (const auto &[term_id, block_offset, score] : term_pairs) {
        if (term_id != last_term_id) {
            if (last_term_id != -1) {
                res.emplace_back(last_term_id, std::move(block_offsets), std::move(scores));
            }
            last_term_id = term_id;
        }
        block_offsets.push_back(block_offset);
        scores.push_back(score);
    }
    if (!block_offsets.empty()) {
        res.emplace_back(last_term_id, std::move(block_offsets), std::move(scores));
    }
    return res;
}

Vector<f32> TailFwd::GetScores(const SparseVecRef<f32, i32> &query) const {
    i8 tail_size = tail_terms_.size();
    Vector<f32> res(tail_size, 0.0);
    for (i8 offset = 0; offset < tail_size; ++offset) {
        const auto &tail_terms = tail_terms_[offset];
        SizeT j = 0;
        for (i32 i = 0; i < query.nnz_; ++i) {
            i32 query_term = query.indices_[i];
            f32 query_score = query.data_[i];
            while (j < tail_terms.size() && tail_terms[j].first < query_term) {
                ++j;
            }
            if (j == tail_terms.size()) {
                break;
            }
            if (tail_terms[j].first == query_term) {
                res[offset] += query_score * tail_terms[j].second;
            }
        }
    }
    return res;
}

Optional<TailFwd> BlockFwd::AddDoc(const SparseVecRef<f32, i32> &doc) {
    i8 tail_size = tail_fwd_.AddDoc(doc);
    if (tail_size < block_size_) {
        return None;
    }
    TailFwd tail_fwd1;
    std::swap(tail_fwd1, tail_fwd_);

    Vector<Tuple<i32, Vector<i8>, Vector<f32>>> block_terms = tail_fwd1.ToBlockFwd();
    block_terms_.emplace_back(std::move(block_terms));
    return tail_fwd1;
}

Vector<Vector<f32>> BlockFwd::GetIvtScores(i32 term_num) const {
    Vector<Vector<f32>> res(term_num);
    for (const auto &block_terms : block_terms_) {
        for (const auto &[term_id, block_offsets, scores] : block_terms) {
            for (SizeT i = 0; i < scores.size(); ++i) {
                res[term_id].push_back(scores[i]);
            }
        }
    }
    return res;
}

Vector<f32> BlockFwd::GetScores(i32 block_id, const SparseVecRef<f32, i32> &query) const {
    const auto &block_terms = block_terms_[block_id];
    // const auto &[term_id, block_offsets, scores] = block_terms[j];
    Vector<f32> res(block_size_, 0.0);
    SizeT j = 0;
    for (i32 i = 0; i < query.nnz_; ++i) {
        i32 query_term = query.indices_[i];
        f32 query_score = query.data_[i];
        while (j < block_terms.size() && std::get<0>(block_terms[j]) < query_term) {
            ++j;
        }
        if (j == block_terms.size()) {
            break;
        }
        if (std::get<0>(block_terms[j]) == query_term) {
            const auto &[_, block_offsets, scores] = block_terms[j];
            BlockFwd::Calculate(block_offsets, scores, res, query_score);
        }
    }
    return res;
}

Vector<f32> BlockFwd::GetScoresTail(const SparseVecRef<f32, i32> &query) const { return tail_fwd_.GetScores(query); }

void BlockFwd::Prefetch(i32 block_id) const {
    const auto *ptr = reinterpret_cast<const char *>(block_terms_[block_id].data());
    _mm_prefetch(ptr, _MM_HINT_T0);
}

template <bool UseSIMD>
void BlockFwd::Calculate(const Vector<i8> &block_offsets, const Vector<f32> scores, Vector<f32> &res, f32 query_score) {
    if constexpr (UseSIMD) {
        UnrecoverableError("Not implemented");
    } else {
        for (SizeT i = 0; i < block_offsets.size(); ++i) {
            i8 block_offset = block_offsets[i];
            res[block_offset] += query_score * scores[i];
        }
    }
}

template void BlockFwd::Calculate<false>(const Vector<i8> &block_offsets, const Vector<f32> scores, Vector<f32> &res, f32 query_score);

void BMIndex::AddDoc(const SparseVecRef<f32, i32> &doc) {
    Optional<TailFwd> tail_fwd = block_fwd_.AddDoc(doc);
    if (!tail_fwd.has_value()) {
        return;
    }
    i32 block_id = block_fwd_.block_num() - 1;
    const auto &tail_terms = tail_fwd->GetTailTerms();
    bm_ivt_.AddBlock(block_id, tail_terms);
}

void BMIndex::Optimize(i32 topk) {
    i32 term_num = bm_ivt_.term_num();
    Vector<Vector<f32>> ivt_scores = block_fwd_.GetIvtScores(term_num);
    bm_ivt_.Optimize(topk, std::move(ivt_scores));
}

template <bool UseLock>
Pair<Vector<i32>, Vector<f32>> BMIndex::SearchKnn(const SparseVecRef<f32, i32> &query, i32 topk, f32 alpha, f32 beta) const {
    SparseVecEle<f32, i32> keeped_query;
    if (beta < 1.0) {
        i32 terms_to_keep = std::ceil(query.nnz_ * beta);
        Vector<i32> query_term_idxes(query.nnz_);
        std::iota(query_term_idxes.begin(), query_term_idxes.end(), 0);
        std::partial_sort(query_term_idxes.begin(), query_term_idxes.begin() + terms_to_keep, query_term_idxes.end(), [&](i32 a, i32 b) {
            return query.data_[a] > query.data_[b];
        });
        query_term_idxes.resize(terms_to_keep);
        std::sort(query_term_idxes.begin(), query_term_idxes.end(), [&](i32 a, i32 b) { return query.indices_[a] < query.indices_[b]; });

        keeped_query.Init(terms_to_keep);
        i32 i = 0;
        for (i32 query_term_idx : query_term_idxes) {
            keeped_query.indices_[i] = query.indices_[query_term_idx];
            keeped_query.data_[i] = query.data_[query_term_idx];
            ++i;
        }
    }
    const SparseVecRef<f32, i32> &query_ref =
        beta < 1.0 ? SparseVecRef<f32, i32>(keeped_query.nnz_, keeped_query.indices_.get(), keeped_query.data_.get()) : query;

    const auto &postings = bm_ivt_.GetPostings();
    f32 threshold = 0.0;
    i32 block_num = block_fwd_.block_num();
    Vector<f32> upper_bounds(block_num, 0.0);
    for (i32 i = 0; i < query_ref.nnz_; ++i) {
        i32 query_term = query_ref.indices_[i];
        f32 query_score = query_ref.data_[i];
        const auto &posting = postings[query_term];
        threshold = std::max(threshold, query_score * posting.kth(topk));
        posting.Calculate<false>(upper_bounds, query_score);
    }

    Vector<Pair<f32, i32>> block_scores;
    for (i32 block_id = 0; block_id < block_num; ++block_id) {
        if (upper_bounds[block_id] >= threshold) {
            block_scores.emplace_back(upper_bounds[block_id], block_id);
        }
    }
    std::sort(block_scores.begin(), block_scores.end(), [](const auto &a, const auto &b) { return a.first > b.first; });

    Vector<i32> result(topk);
    Vector<f32> result_score(topk);
    HeapResultHandler<CompareMin<f32, i32>> result_handler(1 /*query_n*/, topk, result_score.data(), result.data());

    SizeT block_scores_num = block_scores.size();
    for (SizeT i = 0; i < block_scores_num; ++i) {
        if (i + 1 < block_scores_num) {
            i32 next_block_id = block_scores[i + 1].second;
            block_fwd_.Prefetch(next_block_id);
        }
        const auto &[ub_score, block_id] = block_scores[i];
        i32 off = block_id * block_fwd_.block_size();
        Vector<f32> scores = block_fwd_.GetScores(block_id, query_ref);
        for (SizeT block_off = 0; block_off < scores.size(); ++block_off) {
            i32 doc_id = off + block_off;
            f32 score = scores[block_off];
            result_handler.AddResult(0 /*query_id*/, score, doc_id);
        }
        if (ub_score * alpha < result_handler.GetDistance0(0 /*query_id*/)) {
            break;
        }
    }
    Vector<f32> tail_scores = block_fwd_.GetScoresTail(query_ref);
    for (i32 i = 0; i < (i32)tail_scores.size(); ++i) {
        i32 doc_id = block_num * block_fwd_.block_size() + i;
        f32 score = tail_scores[i];
        result_handler.AddResult(0 /*query_id*/, score, doc_id);
    }

    result_handler.End(0 /*query_id*/);
    return {result, result_score};
}

template Pair<Vector<i32>, Vector<f32>> BMIndex::SearchKnn<false>(const SparseVecRef<f32, i32> &query, i32 topk, f32 alpha, f32 beta) const;

} // namespace infinity
