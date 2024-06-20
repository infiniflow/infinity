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

module bmp_alg;

import infinity_exception;
import third_party;
import knn_result_handler;
import serialize;
import segment_iter;

namespace infinity {

template <typename DataType, BMPCompressType CompressType>
template <typename IdxType>
void BMPIvt<DataType, CompressType>::AddBlock(BMPBlockID block_id, const Vector<Vector<Pair<IdxType, DataType>>> &tail_terms) {
    HashMap<IdxType, DataType> max_scores;
    for (const auto &terms : tail_terms) {
        for (const auto &[term_id, score] : terms) {
            max_scores[term_id] = std::max(max_scores[term_id], score);
        }
    }
    for (const auto &[term_id, score] : max_scores) {
        postings_[term_id].data_.AddBlock(block_id, score);
    }
}

template <typename DataType, BMPCompressType CompressType>
void BMPIvt<DataType, CompressType>::Optimize(i32 topk, Vector<Vector<DataType>> ivt_scores) {
    for (SizeT term_id = 0; term_id < ivt_scores.size(); ++term_id) {
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

template class BMPIvt<f32, BMPCompressType::kCompressed>;
template class BMPIvt<f32, BMPCompressType::kRaw>;
template class BMPIvt<f64, BMPCompressType::kCompressed>;
template class BMPIvt<f64, BMPCompressType::kRaw>;

template <typename DataType, typename IdxType>
SizeT TailFwd<DataType, IdxType>::AddDoc(const SparseVecRef<DataType, IdxType> &doc) {
    Vector<Pair<IdxType, DataType>> doc_terms;
    for (i32 i = 0; i < doc.nnz_; ++i) {
        doc_terms.emplace_back(doc.indices_[i], doc.data_[i]);
    }
    tail_terms_.emplace_back(std::move(doc_terms));
    return tail_terms_.size();
}

template <typename DataType, typename IdxType>
Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> TailFwd<DataType, IdxType>::ToBlockFwd() const {
    Vector<Tuple<IdxType, BMPBlockOffset, DataType>> term_pairs;
    SizeT block_size = tail_terms_.size();
    for (SizeT block_offset = 0; block_offset < block_size; ++block_offset) {
        for (const auto &[term_id, score] : tail_terms_[block_offset]) {
            term_pairs.emplace_back(term_id, block_offset, score);
        }
    }
    std::sort(term_pairs.begin(), term_pairs.end(), [](const auto &a, const auto &b) { return std::get<0>(a) < std::get<0>(b); });

    Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> res;
    IdxType last_term_id = -1;
    Vector<BMPBlockOffset> block_offsets;
    Vector<DataType> scores;
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

template <typename DataType, typename IdxType>
Vector<DataType> TailFwd<DataType, IdxType>::GetScores(const SparseVecRef<DataType, IdxType> &query) const {
    SizeT tail_size = tail_terms_.size();
    Vector<DataType> res(tail_size, 0.0);
    for (SizeT offset = 0; offset < tail_size; ++offset) {
        const auto &tail_terms = tail_terms_[offset];
        SizeT j = 0;
        for (i32 i = 0; i < query.nnz_; ++i) {
            IdxType query_term = query.indices_[i];
            DataType query_score = query.data_[i];
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

template class TailFwd<f32, i32>;
template class TailFwd<f32, i16>;
template class TailFwd<f32, i8>;
template class TailFwd<f64, i32>;
template class TailFwd<f64, i16>;
template class TailFwd<f64, i8>;

template <typename DataType, typename IdxType>
Optional<TailFwd<DataType, IdxType>> BlockFwd<DataType, IdxType>::AddDoc(const SparseVecRef<DataType, IdxType> &doc) {
    SizeT tail_size = tail_fwd_.AddDoc(doc);
    if (tail_size < block_size_) {
        return None;
    }
    TailFwd<DataType, IdxType> tail_fwd1;
    std::swap(tail_fwd1, tail_fwd_);

    Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> block_terms = tail_fwd1.ToBlockFwd();
    block_terms_.emplace_back(std::move(block_terms));
    return tail_fwd1;
}

template <typename DataType, typename IdxType>
Vector<Vector<DataType>> BlockFwd<DataType, IdxType>::GetIvtScores(SizeT term_num) const {
    Vector<Vector<DataType>> res(term_num);
    for (const auto &block_terms : block_terms_) {
        for (const auto &[term_id, block_offsets, scores] : block_terms) {
            for (SizeT i = 0; i < scores.size(); ++i) {
                res[term_id].push_back(scores[i]);
            }
        }
    }
    return res;
}

template <typename DataType, typename IdxType>
Vector<DataType> BlockFwd<DataType, IdxType>::GetScores(BMPBlockID block_id, const SparseVecRef<DataType, IdxType> &query) const {
    const auto &block_terms = block_terms_[block_id];
    Vector<DataType> res(block_size_, 0.0);
    SizeT j = 0;
    for (i32 i = 0; i < query.nnz_; ++i) {
        IdxType query_term = query.indices_[i];
        DataType query_score = query.data_[i];
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

template <typename DataType, typename IdxType>
void BlockFwd<DataType, IdxType>::Prefetch(BMPBlockID block_id) const {
    const auto *ptr = reinterpret_cast<const char *>(block_terms_[block_id].data());
    _mm_prefetch(ptr, _MM_HINT_T0);
}

template <typename DataType, typename IdxType>
void BlockFwd<DataType, IdxType>::Calculate(const Vector<BMPBlockOffset> &block_offsets,
                                            const Vector<DataType> scores,
                                            Vector<DataType> &res,
                                            DataType query_score) {
    for (SizeT i = 0; i < block_offsets.size(); ++i) {
        BMPBlockOffset block_offset = block_offsets[i];
        res[block_offset] += query_score * scores[i];
    }
}

template class BlockFwd<f32, i32>;
template class BlockFwd<f32, i16>;
template class BlockFwd<f32, i8>;
template class BlockFwd<f64, i32>;
template class BlockFwd<f64, i16>;
template class BlockFwd<f64, i8>;

template <typename DataType, typename IdxType, BMPCompressType CompressType>
void BMPAlg<DataType, IdxType, CompressType>::AddDoc(const SparseVecRef<DataType, IdxType> &doc, BMPDocID doc_id) {
    doc_ids_.push_back(doc_id);
    Optional<TailFwd<DataType, IdxType>> tail_fwd = block_fwd_.AddDoc(doc);
    if (!tail_fwd.has_value()) {
        return;
    }
    BMPBlockID block_id = block_fwd_.block_num() - 1;
    const auto &tail_terms = tail_fwd->GetTailTerms();
    bm_ivt_.AddBlock(block_id, tail_terms);
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
void BMPAlg<DataType, IdxType, CompressType>::Optimize(i32 topk) {
    SizeT term_num = bm_ivt_.term_num();
    Vector<Vector<DataType>> ivt_scores = block_fwd_.GetIvtScores(term_num);
    bm_ivt_.Optimize(topk, std::move(ivt_scores));
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
Pair<Vector<BMPDocID>, Vector<DataType>>
BMPAlg<DataType, IdxType, CompressType>::SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, f32 alpha, f32 beta) const {
    SizeT block_size = block_fwd_.block_size();
    SparseVecEle<DataType, IdxType> keeped_query;
    if (beta < 1.0) {
        i32 terms_to_keep = std::ceil(query.nnz_ * beta);
        Vector<SizeT> query_term_idxes(query.nnz_);
        std::iota(query_term_idxes.begin(), query_term_idxes.end(), 0);
        std::partial_sort(query_term_idxes.begin(), query_term_idxes.begin() + terms_to_keep, query_term_idxes.end(), [&](SizeT a, SizeT b) {
            return query.data_[a] > query.data_[b];
        });
        query_term_idxes.resize(terms_to_keep);
        std::sort(query_term_idxes.begin(), query_term_idxes.end(), [&](SizeT a, SizeT b) { return query.indices_[a] < query.indices_[b]; });

        keeped_query.Init(query_term_idxes, query.data_, query.indices_);
    }
    const SparseVecRef<DataType, IdxType> &query_ref =
        beta < 1.0 ? SparseVecRef<DataType, IdxType>(keeped_query.nnz_, keeped_query.indices_.get(), keeped_query.data_.get()) : query;

    const auto &postings = bm_ivt_.GetPostings();
    DataType threshold = 0.0;
    SizeT block_num = block_fwd_.block_num();
    Vector<DataType> upper_bounds(block_num, 0.0);
    for (i32 i = 0; i < query_ref.nnz_; ++i) {
        IdxType query_term = query_ref.indices_[i];
        DataType query_score = query_ref.data_[i];
        const auto &posting = postings[query_term];
        threshold = std::max(threshold, query_score * posting.kth(topk));
        posting.data_.Calculate(upper_bounds, query_score);
    }

    Vector<Pair<DataType, BMPBlockID>> block_scores;
    for (SizeT block_id = 0; block_id < block_num; ++block_id) {
        if (upper_bounds[block_id] >= threshold) {
            block_scores.emplace_back(upper_bounds[block_id], block_id);
        }
    }
    std::sort(block_scores.begin(), block_scores.end(), [](const auto &a, const auto &b) { return a.first > b.first; });

    Vector<BMPDocID> result(topk);
    Vector<DataType> result_score(topk);
    HeapResultHandler<CompareMin<DataType, BMPDocID>> result_handler(1 /*query_n*/, topk, result_score.data(), result.data());

    SizeT block_scores_num = block_scores.size();
    for (SizeT i = 0; i < block_scores_num; ++i) {
        if (i + 1 < block_scores_num) {
            BMPBlockID next_block_id = block_scores[i + 1].second;
            block_fwd_.Prefetch(next_block_id);
        }
        const auto &[ub_score, block_id] = block_scores[i];
        BMPDocID off = block_id * block_size;
        Vector<DataType> scores = block_fwd_.GetScores(block_id, query_ref);
        for (SizeT block_off = 0; block_off < scores.size(); ++block_off) {
            BMPDocID doc_id = off + block_off;
            DataType score = scores[block_off];
            result_handler.AddResult(0 /*query_id*/, score, doc_id);
        }
        if (ub_score * alpha < result_handler.GetDistance0(0 /*query_id*/)) {
            break;
        }
    }
    Vector<DataType> tail_scores = block_fwd_.GetScoresTail(query_ref);
    for (SizeT i = 0; i < tail_scores.size(); ++i) {
        BMPDocID doc_id = block_num * block_size + i;
        DataType score = tail_scores[i];
        result_handler.AddResult(0 /*query_id*/, score, doc_id);
    }

    result_handler.End(0 /*query_id*/);
    Vector<BMPDocID> result_docid(topk);
    std::transform(result.begin(), result.end(), result_docid.begin(), [&](BMPDocID doc_id) { return doc_ids_[doc_id]; });
    return {result_docid, result_score};
}

template class BMPAlg<f32, i32, BMPCompressType::kCompressed>;
template class BMPAlg<f32, i32, BMPCompressType::kRaw>;
template class BMPAlg<f32, i16, BMPCompressType::kCompressed>;
template class BMPAlg<f32, i16, BMPCompressType::kRaw>;
template class BMPAlg<f32, i8, BMPCompressType::kCompressed>;
template class BMPAlg<f32, i8, BMPCompressType::kRaw>;

template class BMPAlg<f64, i32, BMPCompressType::kCompressed>;
template class BMPAlg<f64, i32, BMPCompressType::kRaw>;
template class BMPAlg<f64, i16, BMPCompressType::kCompressed>;
template class BMPAlg<f64, i16, BMPCompressType::kRaw>;
template class BMPAlg<f64, i8, BMPCompressType::kCompressed>;
template class BMPAlg<f64, i8, BMPCompressType::kRaw>;

} // namespace infinity
