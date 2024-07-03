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

#include <algorithm>
#include <vector>
#include <xmmintrin.h>

module bmp_alg;

import infinity_exception;
import third_party;
import serialize;
import segment_iter;
import bp_reordering;

namespace infinity {

template <typename DataType, BMPCompressType CompressType>
template <typename IdxType>
void BMPIvt<DataType, CompressType>::AddBlock(BMPBlockID block_id, const Vector<Pair<Vector<IdxType>, Vector<DataType>>> &tail_terms) {
    HashMap<IdxType, DataType> max_scores;
    for (const auto &[indices, data] : tail_terms) {
        SizeT block_size = indices.size();
        for (SizeT i = 0; i < block_size; ++i) {
            IdxType term_id = indices[i];
            DataType score = data[i];
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
        std::nth_element(term_scores.begin(), term_scores.begin() + topk - 1, term_scores.end(), std::greater<>());
        posting.kth_score_ = term_scores[topk - 1];
    }
}

template class BMPIvt<f32, BMPCompressType::kCompressed>;
template class BMPIvt<f32, BMPCompressType::kRaw>;
template class BMPIvt<f64, BMPCompressType::kCompressed>;
template class BMPIvt<f64, BMPCompressType::kRaw>;

template <typename DataType, typename IdxType>
SizeT TailFwd<DataType, IdxType>::AddDoc(const SparseVecRef<DataType, IdxType> &doc) {
    Vector<IdxType> indices;
    Vector<DataType> data;
    indices.reserve(doc.nnz_);
    data.reserve(doc.nnz_);
    for (i32 i = 0; i < doc.nnz_; ++i) {
        indices.push_back(doc.indices_[i]);
        data.push_back(doc.data_[i]);
    }
    tail_terms_.emplace_back(std::move(indices), std::move(data));
    return tail_terms_.size();
}

template <typename DataType, typename IdxType>
Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> TailFwd<DataType, IdxType>::ToBlockFwd() const {
    Vector<Tuple<IdxType, BMPBlockOffset, DataType>> term_pairs;
    SizeT block_size = tail_terms_.size();
    for (SizeT block_offset = 0; block_offset < block_size; ++block_offset) {
        SizeT block_size = tail_terms_[block_offset].first.size();
        for (SizeT i = 0; i < block_size; ++i) {
            IdxType term_id = tail_terms_[block_offset].first[i];
            DataType score = tail_terms_[block_offset].second[i];
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
        const auto &[indices, data] = tail_terms_[offset];
        SizeT j = 0;
        for (i32 i = 0; i < query.nnz_; ++i) {
            IdxType query_term = query.indices_[i];
            DataType query_score = query.data_[i];
            while (j < indices.size() && indices[j] < query_term) {
                ++j;
            }
            if (j == indices.size()) {
                break;
            }
            if (indices[j] == query_term) {
                res[offset] += query_score * data[j];
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
    TailFwd<DataType, IdxType> tail_fwd1(block_size_);
    std::swap(tail_fwd1, tail_fwd_);

    Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> block_terms = tail_fwd1.ToBlockFwd();
    block_terms_list_.emplace_back(block_terms);
    return tail_fwd1;
}

template <typename DataType, typename IdxType>
Vector<Pair<Vector<IdxType>, Vector<DataType>>> BlockFwd<DataType, IdxType>::GetFwd(SizeT doc_num, SizeT term_num) const {
    SizeT doc_n = doc_num / block_size_ * block_size_;
    Vector<Pair<Vector<IdxType>, Vector<DataType>>> fwd(doc_n);
    for (SizeT block_id = 0; block_id < block_terms_list_.size(); ++block_id) {
        const auto &block_terms = block_terms_list_[block_id];
        for (auto iter = block_terms.Iter(); iter.HasNext(); iter.Next()) {
            const auto &[term_id, block_size, block_offsets, scores] = iter.Value();
            for (SizeT i = 0; i < block_size; ++i) {
                BMPDocID doc_id = block_offsets[i] + block_id * block_size_;
                DataType score = scores[i];
                fwd[doc_id].first.push_back(term_id);
                fwd[doc_id].second.push_back(score);
            }
        }
    }
    return fwd;
}

template <typename DataType, typename IdxType>
Vector<Vector<DataType>> BlockFwd<DataType, IdxType>::GetIvtScores(SizeT term_num) const {
    Vector<Vector<DataType>> res(term_num);
    for (const auto &block_terms : block_terms_list_) {
        for (auto iter = block_terms.Iter(); iter.HasNext(); iter.Next()) {
            const auto [term_id, block_size, block_offsets, scores] = iter.Value();
            for (SizeT i = 0; i < block_size; ++i) {
                res[term_id].push_back(scores[i]);
            }
        }
    }
    return res;
}

template <typename DataType, typename IdxType>
Vector<DataType> BlockFwd<DataType, IdxType>::GetScores(BMPBlockID block_id, const SparseVecRef<DataType, IdxType> &query) const {
    const auto &block_terms = block_terms_list_[block_id];
    Vector<DataType> res(block_size_, 0.0);

    i32 i = 0;
    for (auto iter = block_terms.Iter(); iter.HasNext(); iter.Next()) {
        const auto [term_id, block_size, block_offsets, scores] = iter.Value();
        while (i < query.nnz_ && query.indices_[i] < term_id) {
            ++i;
        }
        if (i == query.nnz_) {
            break;
        }
        if (query.indices_[i] == term_id) {
            Calculate(block_size, block_offsets, scores, res, query.data_[i]);
        }
    }
    return res;
}

template <typename DataType, typename IdxType>
void BlockFwd<DataType, IdxType>::Prefetch(BMPBlockID block_id) const {
    block_terms_list_[block_id].Prefetch();
}

template <typename DataType, typename IdxType>
void BlockFwd<DataType, IdxType>::Calculate(SizeT block_size,
                                            const BMPBlockOffset *block_offsets,
                                            const DataType *scores,
                                            Vector<DataType> &res,
                                            DataType query_score) {
    for (SizeT i = 0; i < block_size; ++i) {
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
void BMPAlg<DataType, IdxType, CompressType>::AddDoc(const SparseVecRef<DataType, IdxType> &doc, BMPDocID doc_id, bool lck) {
    std::unique_lock<std::shared_mutex> lock;
    if (lck) {
        lock = std::unique_lock(mtx_);
    }

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
void BMPAlg<DataType, IdxType, CompressType>::Optimize(const BMPOptimizeOptions &options) {
    std::unique_lock lock(mtx_);

    if (options.bp_reorder_) {
        SizeT block_size = block_fwd_.block_size();
        SizeT term_num = bm_ivt_.term_num();
        SizeT doc_num = doc_ids_.size() - doc_ids_.size() % block_size;

        bm_ivt_ = BMPIvt<DataType, CompressType>(term_num);
        Vector<Pair<Vector<IdxType>, Vector<DataType>>> fwd = block_fwd_.GetFwd(doc_num, term_num);
        TailFwd<DataType, IdxType> tail_fwd = block_fwd_.GetTailFwd();
        block_fwd_ = BlockFwd<DataType, IdxType>(block_size);

        BPReordering<IdxType, BMPDocID> bp;
        for (BMPDocID i = 0; i < doc_num; ++i) {
            bp.AddDoc(&fwd[i].first);
        }
        Vector<BMPDocID> remap = bp(term_num);

        Vector<BMPDocID> doc_ids;
        std::swap(doc_ids, doc_ids_);
        for (BMPDocID new_id = 0; new_id < doc_num; ++new_id) {
            BMPDocID old_id = remap[new_id];
            SparseVecRef<DataType, IdxType> doc((i32)fwd[old_id].first.size(), fwd[old_id].first.data(), fwd[old_id].second.data());
            this->AddDoc(doc, doc_ids[old_id], false);
        }
        for (BMPDocID i = doc_num; i < doc_ids.size(); ++i) {
            const auto &[indices, data] = tail_fwd.GetTailTerms()[i - doc_num];
            SparseVecRef<DataType, IdxType> doc((i32)indices.size(), indices.data(), data.data());
            this->AddDoc(doc, doc_ids[i], false);
        }
    }
    if (options.topk_ != 0) {
        SizeT term_num = bm_ivt_.term_num();
        Vector<Vector<DataType>> ivt_scores = block_fwd_.GetIvtScores(term_num);
        bm_ivt_.Optimize(options.topk_, std::move(ivt_scores));
    }
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
Pair<Vector<BMPDocID>, Vector<DataType>>
BMPAlg<DataType, IdxType, CompressType>::SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options) const {
    return SearchKnn(query, topk, options, nullptr);
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
