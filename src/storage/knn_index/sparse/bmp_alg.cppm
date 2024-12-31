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
import local_file_handle;
import bmp_util;
import hnsw_common;
import knn_result_handler;
import bmp_ivt;
import bmp_fwd;
import bp_reordering;
import serialize;
import third_party;
import infinity_exception;

namespace infinity {

export template <typename DataType, typename IdxType, BMPCompressType CompressType, BMPOwnMem OwnMem>
class BMPAlgBase {
public:
protected:
    BMPAlgBase(BMPIvt<DataType, CompressType, OwnMem> bm_ivt, BlockFwd<DataType, IdxType, OwnMem> block_fwd, Vector<BMPDocID> doc_ids)
        : bm_ivt_(std::move(bm_ivt)), block_fwd_(std::move(block_fwd)), doc_ids_(std::move(doc_ids)) {}

public:
    BMPAlgBase() {}

    Pair<Vector<BMPDocID>, Vector<DataType>>
    SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options) const {
        return SearchKnn(query, topk, options, nullptr);
    }

    template <FilterConcept<BMPDocID> Filter = NoneType>
    Pair<Vector<BMPDocID>, Vector<DataType>>
    SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options, const Filter &filter) const {
        if (topk == 0) {
            return {{}, {}};
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
            posting.data().Calculate(upper_bounds, query_score);
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
            const auto &block_terms = block_fwd_.GetBlockTerms(block_id);
            Vector<DataType> scores = GetScores(block_terms, query_ref);
            for (SizeT block_off = 0; block_off < scores.size(); ++block_off) {
                BMPDocID doc_id = off + block_off;
                DataType score = scores[block_off];
                add_result(score, doc_id);
            }
            if (result_handler.GetSize(0) == u32(topk) && ub_score * options.alpha_ < result_handler.GetDistance0(0 /*query_id*/)) {
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

protected:
    Vector<DataType> GetScores(const BlockTerms<DataType, IdxType, OwnMem> &block_terms, const SparseVecRef<DataType, IdxType> &query) const {
        Vector<DataType> res(block_fwd_.block_size(), 0.0);

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

    static void
    Calculate(SizeT block_size, const BMPBlockOffset *block_offsets, const DataType *scores, Vector<DataType> &res, DataType query_score) {
        for (SizeT i = 0; i < block_size; ++i) {
            BMPBlockOffset block_offset = block_offsets[i];
            res[block_offset] += query_score * scores[i];
        }
    }

protected:
    BMPIvt<DataType, CompressType, OwnMem> bm_ivt_;
    BlockFwd<DataType, IdxType, OwnMem> block_fwd_;
    VecPtr<BMPDocID, OwnMem> doc_ids_;
};

export template <typename DataType, typename IdxType, BMPCompressType CompressType, BMPOwnMem OwnMem = BMPOwnMem::kTrue>
class BMPAlg {};

export template <typename DataType, typename IdxType, BMPCompressType CompressType>
class BMPAlg<DataType, IdxType, CompressType, BMPOwnMem::kTrue> : public BMPAlgBase<DataType, IdxType, CompressType, BMPOwnMem::kTrue> {
public:
    using DataT = DataType;
    using IdxT = IdxType;

private:
    BMPAlg(BMPIvt<DataType, CompressType, BMPOwnMem::kTrue> bm_ivt, BlockFwd<DataType, IdxType, BMPOwnMem::kTrue> block_fwd, Vector<BMPDocID> doc_ids)
        : BMPAlgBase<DataType, IdxType, CompressType, BMPOwnMem::kTrue>(std::move(bm_ivt), std::move(block_fwd), std::move(doc_ids)) {}

public:
    BMPAlg(SizeT term_num, SizeT block_size) {};

    void AddDoc(const SparseVecRef<DataType, IdxType> &doc, BMPDocID doc_id, bool lck = true) {
        std::unique_lock<std::shared_mutex> lock;
        if (lck) {
            lock = std::unique_lock(mtx_);
        }

        SizeT mem_usage = 0;
        this->doc_ids_.push_back(doc_id);
        Optional<TailFwd<DataType, IdxType>> tail_fwd = this->block_fwd_.AddDoc(doc, mem_usage);
        if (!tail_fwd.has_value()) {
            mem_usage_.fetch_add(sizeof(BMPDocID) + mem_usage);
            return;
        }
        BMPBlockID block_id = this->block_fwd_.block_num() - 1;
        const auto &tail_terms = tail_fwd->GetTailTerms();
        this->bm_ivt_.AddBlock(block_id, tail_terms, mem_usage);
        mem_usage_.fetch_add(sizeof(BMPDocID) + mem_usage);
    }

    template <DataIteratorConcept<SparseVecRef<DataType, IdxType>, BMPDocID> Iterator>
    SizeT AddDocs(Iterator iter) {
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

    SizeT DocNum() const {
        std::shared_lock lock(mtx_);
        return this->doc_ids_.size();
    }

    void Optimize(const BMPOptimizeOptions &options) {
        std::unique_lock lock(mtx_);

        if (options.bp_reorder_) {
            SizeT block_size = this->block_fwd_.block_size();
            SizeT term_num = this->bm_ivt_.term_num();
            SizeT doc_num = this->doc_ids_.size() - this->doc_ids_.size() % block_size;

            this->bm_ivt_ = BMPIvt<DataType, CompressType, BMPOwnMem::kTrue>(term_num);
            Vector<Pair<Vector<IdxType>, Vector<DataType>>> fwd = this->block_fwd_.GetFwd(doc_num, term_num);
            TailFwd<DataType, IdxType> tail_fwd = this->block_fwd_.GetTailFwd();
            this->block_fwd_ = BlockFwd<DataType, IdxType, BMPOwnMem::kTrue>(block_size);

            BPReordering<IdxType, BMPDocID> bp(term_num);
            // add bp parameter here
            for (BMPDocID i = 0; i < doc_num; ++i) {
                bp.AddDoc(&fwd[i].first);
            }
            Vector<BMPDocID> remap = bp();

            Vector<BMPDocID> doc_ids = this->doc_ids_.exchange(Vector<BMPDocID>());
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
            SizeT term_num = this->bm_ivt_.term_num();
            Vector<Vector<DataType>> ivt_scores = this->block_fwd_.GetIvtScores(term_num);
            this->bm_ivt_.Optimize(options.topk_, std::move(ivt_scores));
        }
    }

    Pair<Vector<BMPDocID>, Vector<DataType>>
    SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options) const {
        return SearchKnn(query, topk, options, nullptr);
    }

    template <FilterConcept<BMPDocID> Filter = NoneType>
    Pair<Vector<BMPDocID>, Vector<DataType>>
    SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options, const Filter &filter) const {
        std::shared_lock lock(mtx_, std::defer_lock);
        if (options.use_lock_) {
            lock.lock();
        }
        return BMPAlgBase<DataType, IdxType, CompressType, BMPOwnMem::kTrue>::SearchKnn(query, topk, options, filter);
    }

    void Save(LocalFileHandle &file_handle) const {
        auto size = GetSizeInBytes();
        auto buffer = MakeUnique<char[]>(sizeof(size) + size);
        char *p = buffer.get();
        WriteBufAdv<SizeT>(p, size);
        WriteAdv(p);
        if (SizeT write_n = p - buffer.get(); write_n != sizeof(size) + size) {
            UnrecoverableError(fmt::format("BMPAlg::Save: write_n != sizeof(size) + size: {} != {}", write_n, sizeof(size) + size));
        }
        file_handle.Append(buffer.get(), sizeof(size) + size);
    }

    static BMPAlg<DataType, IdxType, CompressType> Load(LocalFileHandle &file_handle) {
        SizeT size;
        file_handle.Read(&size, sizeof(size));
        auto buffer = MakeUnique<char[]>(size);
        file_handle.Read(buffer.get(), size);
        const char *p = buffer.get();
        return ReadAdv(p);
    }

    SizeT GetSizeInBytes() const {
        std::shared_lock lock(mtx_);

        SizeT size = 0;
        size += this->bm_ivt_.GetSizeInBytes();
        size += this->block_fwd_.GetSizeInBytes();
        size += sizeof(SizeT);
        size += this->doc_ids_.size() * sizeof(BMPDocID);
        return size;
    }

    inline SizeT MemoryUsage() const { return mem_usage_.load(); }

    void SaveToPtr(LocalFileHandle &file_handle) const {
        char *p0 = nullptr;
        GetSizeToPtr(p0);
        char *p1 = nullptr;
        SizeT size = p0 - p1;
        auto buffer = MakeUnique<char[]>(size);
    }

private:
    void GetSizeToPtr(char *&p) const {
        //
    }

    void WriteAdv(char *&p) const {
        std::shared_lock lock(mtx_);

        this->bm_ivt_.WriteAdv(p);
        this->block_fwd_.WriteAdv(p);
        SizeT doc_num = this->doc_ids_.size();
        WriteBufAdv<SizeT>(p, doc_num);
        WriteBufVecAdv(p, this->doc_ids_.data(), this->doc_ids_.size());
    }

    static BMPAlg<DataType, IdxType, CompressType> ReadAdv(const char *&p) {
        auto postings = BMPIvt<DataType, CompressType, BMPOwnMem::kTrue>::ReadAdv(p);
        auto block_fwd = BlockFwd<DataType, IdxType, BMPOwnMem::kTrue>::ReadAdv(p);
        SizeT doc_num = ReadBufAdv<SizeT>(p);
        Vector<BMPDocID> doc_ids(doc_num);
        for (SizeT i = 0; i < doc_num; ++i) {
            doc_ids[i] = ReadBufAdv<BMPDocID>(p);
        }
        return BMPAlg(std::move(postings), std::move(block_fwd), std::move(doc_ids));
    }

private:
    Atomic<SizeT> mem_usage_ = 0;

    mutable std::shared_mutex mtx_;
};

export template <typename DataType, typename IdxType, BMPCompressType CompressType>
class BMPAlg<DataType, IdxType, CompressType, BMPOwnMem::kFalse> : public BMPAlgBase<DataType, IdxType, CompressType, BMPOwnMem::kFalse> {
public:
    static BMPAlg<DataType, IdxType, CompressType, BMPOwnMem::kFalse> LoadFromPtr(const char *&p, SizeT size) {
        //
        return {};
    }

private:
};

} // namespace infinity
