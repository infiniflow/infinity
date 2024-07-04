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

export module bmp_alg;

import stl;
import sparse_util;
import file_system;
import bm_posting;
import bmp_util;
import hnsw_common;
import knn_result_handler;
import bmp_blockterms;

namespace infinity {

template <typename DataType, BMPCompressType CompressType>
class BMPIvt {
private:
    BMPIvt(Vector<BlockPostings<DataType, CompressType>> postings) : postings_(std::move(postings)) {}

public:
    BMPIvt(SizeT term_num) : postings_(term_num) {}

    template <typename IdxType>
    void AddBlock(BMPBlockID block_id, const Vector<Pair<Vector<IdxType>, Vector<DataType>>> &tail_terms);

    void Optimize(i32 topk, Vector<Vector<DataType>> ivt_scores);

    const BlockPostings<DataType, CompressType> &GetPostings(SizeT term_id) const { return postings_[term_id]; }

    void Prefetch(SizeT term_id) const { postings_[term_id].Prefetch(); }

    SizeT term_num() const { return postings_.size(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BMPIvt ReadAdv(const char *&p);

private:
    Vector<BlockPostings<DataType, CompressType>> postings_;
};

template <typename DataType, typename IdxType>
class TailFwd {
private:
    TailFwd(Vector<Pair<Vector<IdxType>, Vector<DataType>>> tail_terms) : tail_terms_(std::move(tail_terms)) {}

public:
    TailFwd(SizeT block_size) { tail_terms_.reserve(block_size); }

    SizeT AddDoc(const SparseVecRef<DataType, IdxType> &doc);

    const Vector<Pair<Vector<IdxType>, Vector<DataType>>> &GetTailTerms() const { return tail_terms_; }

    Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> ToBlockFwd() const;

    Vector<DataType> GetScores(const SparseVecRef<DataType, IdxType> &query) const;

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static TailFwd<DataType, IdxType> ReadAdv(const char *&p);

private:
    Vector<Pair<Vector<IdxType>, Vector<DataType>>> tail_terms_;
};

template <typename DataType, typename IdxType>
class BlockFwd {
public:
    BlockFwd(SizeT block_size, Vector<BlockTerms<DataType, IdxType>> block_terms_list, TailFwd<DataType, IdxType> tail_fwd)
        : block_size_(block_size), block_terms_list_(std::move(block_terms_list)), tail_fwd_(std::move(tail_fwd)) {}

    BlockFwd(SizeT block_size) : block_size_(block_size), tail_fwd_(block_size) {}

    Optional<TailFwd<DataType, IdxType>> AddDoc(const SparseVecRef<DataType, IdxType> &doc);

    Vector<Pair<Vector<IdxType>, Vector<DataType>>> GetFwd(SizeT doc_num, SizeT term_num) const;

    TailFwd<DataType, IdxType> GetTailFwd() { return std::move(tail_fwd_); }

    Vector<Vector<DataType>> GetIvtScores(SizeT term_num) const;

    Vector<DataType> GetScores(BMPBlockID block_id, const SparseVecRef<DataType, IdxType> &query) const;

    Vector<DataType> GetScoresTail(const SparseVecRef<DataType, IdxType> &query) const { return tail_fwd_.GetScores(query); }

    void Prefetch(BMPBlockID block_id) const;

    SizeT block_size() const { return block_size_; }

    SizeT block_num() const { return block_terms_list_.size(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockFwd ReadAdv(const char *&p);

private:
    static void Calculate(SizeT block_size, const BMPBlockOffset *block_offsets, const DataType *scores, Vector<DataType> &res, DataType query_score);

private:
    SizeT block_size_;
    Vector<BlockTerms<DataType, IdxType>> block_terms_list_;
    TailFwd<DataType, IdxType> tail_fwd_;
};

export template <typename DataType, typename IdxType, BMPCompressType CompressType>
class BMPAlg {
public:
    using DataT = DataType;
    using IdxT = IdxType;

private:
    BMPAlg(BMPIvt<DataType, CompressType> bm_ivt, BlockFwd<DataType, IdxType> block_fwd, Vector<BMPDocID> doc_ids)
        : bm_ivt_(std::move(bm_ivt)), block_fwd_(std::move(block_fwd)), doc_ids_(std::move(doc_ids)) {}

public:
    BMPAlg(SizeT term_num, SizeT block_size) : bm_ivt_(term_num), block_fwd_(block_size) {}

    void AddDoc(const SparseVecRef<DataType, IdxType> &doc, BMPDocID doc_id, bool lock = true);

    template <DataIteratorConcept<SparseVecRef<DataType, IdxType>, BMPDocID> Iterator>
    SizeT AddDocs(Iterator iter);

    SizeT DocNum() const;

    void Optimize(const BMPOptimizeOptions &options);

    Pair<Vector<BMPDocID>, Vector<DataType>> SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options) const;

    template <FilterConcept<BMPDocID> Filter = NoneType>
    Pair<Vector<BMPDocID>, Vector<DataType>>
    SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options, const Filter &filter) const;

    void Save(FileHandler &file_handler) const;

    static BMPAlg<DataType, IdxType, CompressType> Load(FileHandler &file_handler);

private:
    SizeT GetSizeInBytes() const;

    void WriteAdv(char *&p) const;

    static BMPAlg<DataType, IdxType, CompressType> ReadAdv(const char *&p);

private:
    BMPIvt<DataType, CompressType> bm_ivt_;
    BlockFwd<DataType, IdxType> block_fwd_;
    Vector<BMPDocID> doc_ids_;

    mutable std::shared_mutex mtx_;
};

template <typename DataType, typename IdxType, BMPCompressType CompressType>
template <DataIteratorConcept<SparseVecRef<DataType, IdxType>, BMPDocID> Iterator>
SizeT BMPAlg<DataType, IdxType, CompressType>::AddDocs(Iterator iter) {
    SizeT cnt = 0;
    while (true) {
        auto ret = iter.Next();
        if (!ret.has_value()) {
            break;
        }
        const auto &[sparse_ref, doc_id] = *ret;
        AddDoc(sparse_ref, doc_id);
        ++cnt;
    }
    return cnt;
}

template <typename DataType, typename IdxType, BMPCompressType CompressType>
template <FilterConcept<BMPDocID> Filter>
Pair<Vector<BMPDocID>, Vector<DataType>> BMPAlg<DataType, IdxType, CompressType>::SearchKnn(const SparseVecRef<DataType, IdxType> &query,
                                                                                            i32 topk,
                                                                                            const BmpSearchOptions &options,
                                                                                            const Filter &filter) const {
    std::shared_lock lock(mtx_, std::defer_lock);
    if (options.use_lock_) {
        lock.lock();
    }

    SizeT block_size = block_fwd_.block_size();
    SparseVecEle<DataType, IdxType> keeped_query;
    if (options.beta_ < 1.0) {
        i32 terms_to_keep = std::ceil(query.nnz_ * options.beta_);
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
        options.beta_ < 1.0 ? SparseVecRef<DataType, IdxType>(keeped_query.nnz_, keeped_query.indices_.get(), keeped_query.data_.get()) : query;

    DataType threshold = 0.0;
    SizeT block_num = block_fwd_.block_num();
    Vector<DataType> upper_bounds(block_num, 0.0);
    for (i32 i = 0; i < query_ref.nnz_; ++i) {
        // if (i + 1 < query_ref.nnz_) {
        //     IdxType next_query_term = query_ref.indices_[i + 1];
        //     bm_ivt_.Prefetch(next_query_term);
        // }
        IdxType query_term = query_ref.indices_[i];
        DataType query_score = query_ref.data_[i];
        const auto &posting = bm_ivt_.GetPostings(query_term);
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

    auto add_result = [&](DataType score, BMPDocID doc_id) {
        if constexpr (std::is_same_v<Filter, std::nullptr_t>) {
            result_handler.AddResult(0 /*query_id*/, score, doc_id);
        } else {
            if (filter(doc_ids_[doc_id])) {
                result_handler.AddResult(0 /*query_id*/, score, doc_id);
            }
        }
    };

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
            add_result(score, doc_id);
        }
        if (ub_score * options.alpha_ < result_handler.GetDistance0(0 /*query_id*/)) {
            break;
        }
    }

    if (options.use_tail_) {
        Vector<DataType> tail_scores = block_fwd_.GetScoresTail(query_ref);
        for (SizeT i = 0; i < tail_scores.size(); ++i) {
            BMPDocID doc_id = block_num * block_size + i;
            DataType score = tail_scores[i];
            add_result(score, doc_id);
        }
    }

    SizeT res_n = result_handler.GetSize(0 /*query_id*/);
    result_handler.End(0 /*query_id*/);
    result.erase(result.begin() + res_n, result.end());
    result_score.erase(result_score.begin() + res_n, result_score.end());
    Vector<BMPDocID> result_docid(res_n);
    std::transform(result.begin(), result.end(), result_docid.begin(), [&](BMPDocID doc_id) { return doc_ids_[doc_id]; });
    return {result_docid, result_score};
}

} // namespace infinity
