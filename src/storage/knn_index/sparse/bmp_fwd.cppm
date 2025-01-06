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

#include "common/simd/simd_common_intrin_include.h"
#include <vector>

export module bmp_fwd;

import stl;
import sparse_util;
import local_file_handle;
import bmp_util;
import knn_result_handler;
import serialize;
import infinity_exception;

namespace infinity {

template <typename DataType, typename IdxType, BMPOwnMem OwnMem>
class BlockTermsIter;

template <typename DataType, typename IdxType>
class BlockTermsIter<DataType, IdxType, BMPOwnMem::kTrue> {
public:
    BlockTermsIter(const char *ptr, const char *ptr_end) : ptr_(ptr), ptr_end_(ptr_end) {}

    bool HasNext() const { return ptr_ < ptr_end_; }

    Tuple<IdxType, SizeT, const BMPBlockOffset *, const DataType *> Value() const {
        const char *p = ptr_;
        auto block_size = ReadBufAdv<SizeT>(p);
        auto term_id = ReadBufAdv<IdxType>(p);

        auto *block_offsets = reinterpret_cast<const BMPBlockOffset *>(p);
        auto *values = reinterpret_cast<const DataType *>(block_offsets + block_size);
        return {term_id, block_size, block_offsets, values};
    }

    void Next() {
        auto block_size = ReadBuf<SizeT>(ptr_);
        ptr_ += sizeof(block_size) + sizeof(IdxType) + block_size * sizeof(BMPBlockOffset) + block_size * sizeof(DataType);
    }

private:
    const char *ptr_;
    const char *ptr_end_;
};

template <typename DataType, typename IdxType>
class BlockTermsIter<DataType, IdxType, BMPOwnMem::kFalse> {
public:
    BlockTermsIter(SizeT block_term_remain,
                   const SizeT *block_size_prefix_sum,
                   const IdxType *term_ids,
                   const BMPBlockOffset *block_offsets,
                   const DataType *values)
        : block_term_remain_(block_term_remain), block_size_prefix_sum_(block_size_prefix_sum), term_ids_(term_ids), block_offsets_(block_offsets),
          values_(values) {}

    bool HasNext() const { return block_term_remain_ > 0; }

    Tuple<IdxType, SizeT, const BMPBlockOffset *, const DataType *> Value() const {
        SizeT block_size = block_size_prefix_sum_[1] - block_size_prefix_sum_[0];
        return {term_ids_[0], block_size, block_offsets_, values_};
    }

    void Next() {
        SizeT block_size = block_size_prefix_sum_[1] - block_size_prefix_sum_[0];
        ++block_size_prefix_sum_;
        ++term_ids_;
        block_offsets_ += block_size;
        values_ += block_size;
        --block_term_remain_;
    }

private:
    SizeT block_term_remain_{};
    const SizeT *block_size_prefix_sum_;
    const IdxType *term_ids_;
    const BMPBlockOffset *block_offsets_;
    const DataType *values_;
};

export template <typename DataType, typename IdxType, BMPOwnMem OwnMem>
class BlockTerms {};

export template <typename DataType, typename IdxType>
class BlockTerms<DataType, IdxType, BMPOwnMem::kTrue> {
    using IterT = BlockTermsIter<DataType, IdxType, BMPOwnMem::kTrue>;

public:
    BlockTerms(const Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> &block_terms)
        : data_len_(GetBufferSize(block_terms)), data_(MakeUniqueForOverwrite<char[]>(data_len_)) {
        char *ptr = data_.get();
        for (const auto &[term_id, block_offsets, values] : block_terms) {
            SizeT block_size = block_offsets.size();
            WriteBufAdv(ptr, block_size);
            WriteBufAdv(ptr, term_id);
            WriteBufVecAdv(ptr, block_offsets.data(), block_size);
            WriteBufVecAdv(ptr, values.data(), block_size);
        }
    }

private:
    static SizeT GetBufferSize(const Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> &block_terms) {
        SizeT size = 0;
        for (const auto &[term_id, block_offsets, values] : block_terms) {
            SizeT block_size = block_offsets.size();
            if (block_size != values.size()) {
                UnrecoverableError("Block offsets and values size mismatch");
            }
            size += sizeof(block_size);
            size += sizeof(term_id);
            size += block_size * sizeof(BMPBlockOffset);
            size += block_size * sizeof(DataType);
        }
        return size;
    }

    BlockTerms(SizeT data_len, UniquePtr<char[]> data) : data_len_(data_len), data_(std::move(data)) {}

public:
    IterT Iter() const { return IterT(data_.get(), data_.get() + data_len_); }

    SizeT GetSizeInBytes() const { return sizeof(data_len_) + data_len_; }

    void WriteAdv(char *&p) const {
        WriteBufAdv(p, data_len_);
        WriteBufVecAdv(p, data_.get(), data_len_);
    }

    static BlockTerms<DataType, IdxType, BMPOwnMem::kTrue> ReadAdv(const char *&p) {
        SizeT data_len = ReadBufAdv<SizeT>(p);
        UniquePtr<char[]> data = MakeUniqueForOverwrite<char[]>(data_len);
        std::memcpy(data.get(), p, data_len);
        p += data_len;
        return BlockTerms<DataType, IdxType, BMPOwnMem::kTrue>(data_len, std::move(data));
    }

    void GetSizeToPtr(char *&p) const {
        GetSizeInBytesAligned<SizeT>(p);
        SizeT block_size_sum = 0;
        SizeT cnt = 0;
        for (auto iter = Iter(); iter.HasNext(); iter.Next()) {
            auto [term_id, block_size, block_offsets, values] = iter.Value();
            block_size_sum += block_size;
            ++cnt;
        }
        GetSizeInBytesVecAligned<SizeT>(p, cnt + 1);
        GetSizeInBytesVecAligned<IdxType>(p, cnt);
        GetSizeInBytesVecAligned<BMPBlockOffset>(p, block_size_sum);
        GetSizeInBytesVecAligned<DataType>(p, block_size_sum);
    }

    void WriteToPtr(char *&p) const {
        SizeT block_size_sum = 0;
        Vector<SizeT> block_size_prefix_sum;
        Vector<IdxType> term_ids;
        Vector<BMPBlockOffset> block_offsets_vec;
        Vector<DataType> values_vec;
        block_size_prefix_sum.push_back(0);
        for (auto iter = Iter(); iter.HasNext(); iter.Next()) {
            auto [term_id, block_size, block_offsets, values] = iter.Value();
            block_size_sum += block_size;
            block_size_prefix_sum.push_back(block_size_sum);
            term_ids.push_back(term_id);
            for (SizeT i = 0; i < block_size; ++i) {
                block_offsets_vec.push_back(block_offsets[i]);
                values_vec.push_back(values[i]);
            }
        }
        WriteBufAdvAligned<SizeT>(p, term_ids.size());
        WriteBufVecAdvAligned<SizeT>(p, block_size_prefix_sum.data(), block_size_prefix_sum.size());
        WriteBufVecAdvAligned<IdxType>(p, term_ids.data(), term_ids.size());
        WriteBufVecAdvAligned<BMPBlockOffset>(p, block_offsets_vec.data(), block_offsets_vec.size());
        WriteBufVecAdvAligned<DataType>(p, values_vec.data(), values_vec.size());
    }

    void Prefetch() const {
        const char *ptr = data_.get();
        _mm_prefetch(ptr, _MM_HINT_T0);
    }

private:
    SizeT data_len_{};
    UniquePtr<char[]> data_{};
};

export template <typename DataType, typename IdxType>
class BlockTerms<DataType, IdxType, BMPOwnMem::kFalse> {
    using IterT = BlockTermsIter<DataType, IdxType, BMPOwnMem::kFalse>;

public:
    IterT Iter() const { return IterT(block_term_num_, block_size_prefix_sum_, term_ids_, block_offsets_, values_); }

    static BlockTerms ReadFromPtr(const char *&p) {
        BlockTerms<DataType, IdxType, BMPOwnMem::kFalse> ret;
        ret.block_term_num_ = ReadBufAdvAligned<SizeT>(p);
        ret.block_size_prefix_sum_ = ReadBufVecAdvAligned<SizeT>(p, ret.block_term_num_ + 1);
        ret.term_ids_ = ReadBufVecAdvAligned<IdxType>(p, ret.block_term_num_);
        SizeT block_size_sum = ret.block_size_prefix_sum_[ret.block_term_num_];
        ret.block_offsets_ = ReadBufVecAdvAligned<BMPBlockOffset>(p, block_size_sum);
        ret.values_ = ReadBufVecAdvAligned<DataType>(p, block_size_sum);
        return ret;
    }

    void Prefetch() const {
        _mm_prefetch(block_size_prefix_sum_, _MM_HINT_T0);
        _mm_prefetch(term_ids_, _MM_HINT_T0);
        _mm_prefetch(block_offsets_, _MM_HINT_T0);
        _mm_prefetch(values_, _MM_HINT_T0);
    }

private:
    SizeT block_term_num_;
    const SizeT *block_size_prefix_sum_;
    const IdxType *term_ids_;
    const BMPBlockOffset *block_offsets_;
    const DataType *values_;
};

export template <typename DataType, typename IdxType>
class TailFwd {
private:
    TailFwd(Vector<Pair<Vector<IdxType>, Vector<DataType>>> tail_terms) : tail_terms_(std::move(tail_terms)) {}

public:
    TailFwd() = default;
    TailFwd(SizeT block_size) { tail_terms_.reserve(block_size); }

    SizeT AddDoc(const SparseVecRef<DataType, IdxType> &doc) {
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

    const Vector<Pair<Vector<IdxType>, Vector<DataType>>> &GetTailTerms() const { return tail_terms_; }

    Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> ToBlockFwd() const {
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

    Vector<DataType> GetScores(const SparseVecRef<DataType, IdxType> &query) const {
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

    SizeT GetSizeInBytes() const {
        SizeT size = sizeof(SizeT);
        for (const auto &tail_terms : tail_terms_) {
            const auto [indices, data] = tail_terms;
            size += sizeof(SizeT) + indices.size() * sizeof(IdxType) + data.size() * sizeof(DataType);
        }
        return size;
    }

    void WriteAdv(char *&p) const {
        SizeT tail_num = tail_terms_.size();
        WriteBufAdv<SizeT>(p, tail_num);
        for (const auto &tail_terms : tail_terms_) {
            const auto &[indices, data] = tail_terms;
            SizeT term_num = indices.size();
            WriteBufAdv<SizeT>(p, term_num);
            WriteBufVecAdv(p, indices.data(), indices.size());
            WriteBufVecAdv(p, data.data(), data.size());
        }
    }

    static TailFwd<DataType, IdxType> ReadAdv(const char *&p) {
        SizeT tail_num = ReadBufAdv<SizeT>(p);
        Vector<Pair<Vector<IdxType>, Vector<DataType>>> tail_terms(tail_num);
        for (SizeT i = 0; i < tail_num; ++i) {
            SizeT term_num = ReadBufAdv<SizeT>(p);
            auto &[indices, data] = tail_terms[i];
            indices.resize(term_num);
            data.resize(term_num);
            for (SizeT j = 0; j < term_num; ++j) {
                indices[j] = ReadBufAdv<IdxType>(p);
            }
            for (SizeT j = 0; j < term_num; ++j) {
                data[j] = ReadBufAdv<DataType>(p);
            }
        }
        return TailFwd(std::move(tail_terms));
    }

private:
    Vector<Pair<Vector<IdxType>, Vector<DataType>>> tail_terms_;
};

export template <typename DataType, typename IdxType, BMPOwnMem OwnMem>
class BlockFwd;

export template <typename DataType, typename IdxType>
class BlockFwd<DataType, IdxType, BMPOwnMem::kTrue> {
public:
    BlockFwd() = default;

    BlockFwd(SizeT block_size, Vector<BlockTerms<DataType, IdxType, BMPOwnMem::kTrue>> block_terms_list, TailFwd<DataType, IdxType> tail_fwd)
        : block_size_(block_size), block_terms_list_(std::move(block_terms_list)), tail_fwd_(std::move(tail_fwd)) {}

    BlockFwd(SizeT block_size) : block_size_(block_size), tail_fwd_(block_size) {}

    Optional<TailFwd<DataType, IdxType>> AddDoc(const SparseVecRef<DataType, IdxType> &doc, SizeT &mem_usage) {
        SizeT tail_size = tail_fwd_.AddDoc(doc);
        if (tail_size < block_size_) {
            return None;
        }
        TailFwd<DataType, IdxType> tail_fwd1(block_size_);
        std::swap(tail_fwd1, tail_fwd_);

        Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> block_terms = tail_fwd1.ToBlockFwd();
        block_terms_list_.emplace_back(block_terms);
        mem_usage += block_terms_list_.back().GetSizeInBytes();
        return tail_fwd1;
    }

    Optional<TailFwd<DataType, IdxType>> Finalize() {
        if (tail_fwd_.GetTailTerms().size() == 0) {
            return None;
        }
        Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> block_terms = tail_fwd_.ToBlockFwd();
        block_terms_list_.emplace_back(block_terms);
        return std::move(tail_fwd_);
    }

    Vector<Pair<Vector<IdxType>, Vector<DataType>>> GetFwd(SizeT doc_num, SizeT term_num) const {
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

    TailFwd<DataType, IdxType> GetTailFwd() { return std::move(tail_fwd_); }

    Vector<Vector<DataType>> GetIvtScores(SizeT term_num) const {
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

    const BlockTerms<DataType, IdxType, BMPOwnMem::kTrue> &GetBlockTerms(BMPBlockID block_id) const { return block_terms_list_[block_id]; }

    Vector<DataType> GetScoresTail(const SparseVecRef<DataType, IdxType> &query) const { return tail_fwd_.GetScores(query); }

    void Prefetch(BMPBlockID block_id) const { this->block_terms_list_[block_id].Prefetch(); }

    SizeT block_size() const { return block_size_; }

    SizeT block_num() const { return block_terms_list_.size(); }

    SizeT GetSizeInBytes() const {
        SizeT size = sizeof(this->block_size_) + sizeof(SizeT);
        for (const auto &block_terms : this->block_terms_list_) {
            size += block_terms.GetSizeInBytes();
        }
        size += tail_fwd_.GetSizeInBytes();
        return size;
    }

    void WriteAdv(char *&p) const {
        WriteBufAdv<SizeT>(p, this->block_size_);
        SizeT block_num = this->block_terms_list_.size();
        WriteBufAdv<SizeT>(p, block_num);
        for (const auto &block_terms : this->block_terms_list_) {
            block_terms.WriteAdv(p);
        }
        tail_fwd_.WriteAdv(p);
    }

    static BlockFwd ReadAdv(const char *&p) {
        SizeT block_size = ReadBufAdv<SizeT>(p);
        SizeT block_num = ReadBufAdv<SizeT>(p);
        Vector<BlockTerms<DataType, IdxType, BMPOwnMem::kTrue>> block_terms_list;
        block_terms_list.reserve(block_num);
        for (SizeT i = 0; i < block_num; ++i) {
            auto block_terms = BlockTerms<DataType, IdxType, BMPOwnMem::kTrue>::ReadAdv(p);
            block_terms_list.push_back(std::move(block_terms));
        }
        auto tail_fwd = TailFwd<DataType, IdxType>::ReadAdv(p);
        return BlockFwd(block_size, std::move(block_terms_list), std::move(tail_fwd));
    }

    void GetSizeToPtr(char *&p) const {
        GetSizeInBytesAligned<SizeT>(p);
        GetSizeInBytesAligned<SizeT>(p);
        GetSizeInBytesVecAligned<SizeT>(p, block_terms_list_.size() + 1);
        for (const auto &block_terms : block_terms_list_) {
            block_terms.GetSizeToPtr(p);
        }
    }

    void WriteToPtr(char *&p) const {
        WriteBufAdvAligned<SizeT>(p, block_size_);
        WriteBufAdvAligned<SizeT>(p, block_terms_list_.size());
        Vector<SizeT> offsets;
        {
            char *p1 = p;
            GetSizeInBytesVecAligned<SizeT>(p1, block_terms_list_.size());
            char *p2 = p1;
            offsets.push_back(0);
            for (const auto &block_terms : block_terms_list_) {
                block_terms.GetSizeToPtr(p2);
                offsets.push_back(p2 - p1);
            }
        }
        WriteBufVecAdvAligned<SizeT>(p, offsets.data(), offsets.size());
        for (const auto &block_terms : block_terms_list_) {
            block_terms.WriteToPtr(p);
        }
    }

private:
    SizeT block_size_ = 0;
    Vector<BlockTerms<DataType, IdxType, BMPOwnMem::kTrue>> block_terms_list_;
    TailFwd<DataType, IdxType> tail_fwd_;
};

export template <typename DataType, typename IdxType>
class BlockFwd<DataType, IdxType, BMPOwnMem::kFalse> {
public:
    BlockFwd() = default;

    static BlockFwd LoadFromPtr(const char *&p) {
        BlockFwd<DataType, IdxType, BMPOwnMem::kFalse> ret;
        ret.block_size_ = ReadBufAdvAligned<SizeT>(p);
        ret.block_num_ = ReadBufAdvAligned<SizeT>(p);
        ret.offsets_ = ReadBufVecAdvAligned<SizeT>(p, ret.block_num_ + 1);
        ret.data_ = p;
        p += ret.offsets_[ret.block_num_];
        return ret;
    }

    BlockTerms<DataType, IdxType, BMPOwnMem::kFalse> GetBlockTerms(BMPBlockID block_id) const {
        SizeT offset = offsets_[block_id];
        const char *ptr = data_ + offset;
        return BlockTerms<DataType, IdxType, BMPOwnMem::kFalse>::ReadFromPtr(ptr);
    }

    Vector<DataType> GetScoresTail(const SparseVecRef<DataType, IdxType> &query) const { return {}; }

    void Prefetch(BMPBlockID block_id) const {
        SizeT offset = offsets_[block_id];
        const char *ptr = data_ + offset;
        BlockTerms<DataType, IdxType, BMPOwnMem::kFalse>::ReadFromPtr(ptr).Prefetch();
    }

    SizeT block_size() const { return block_size_; }
    SizeT block_num() const { return block_num_; }

private:
    SizeT block_size_ = 0;
    SizeT block_num_ = 0;
    const SizeT *offsets_ = nullptr;
    const char *data_ = nullptr;
};

} // namespace infinity