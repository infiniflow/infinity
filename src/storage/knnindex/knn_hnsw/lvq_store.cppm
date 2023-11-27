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

#include "storage/knnindex/header.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <new>
#include <type_traits>

import stl;
import hnsw_common;
import file_system;
import plain_store;

export module lvq_store;

namespace infinity {

export template <typename DataT, typename CompressT>
class LVQStore {
public:
    // Compress type must be i8 temporarily
    static_assert(std::is_same<CompressT, i8>());

    using This = LVQStore<DataT, CompressT>;
    using InitArgs = Pair<SizeT, bool>; // first is buffer size, second is whether to make padding
    using DataType = DataT;
    class LVQData;
    using RtnType = LVQData;
    class QueryCtx;
    using QueryCtxType = QueryCtx;

    static constexpr SizeT ERR_IDX = DataStoreMeta::ERR_IDX;
    static constexpr SizeT PADDING_SIZE = 32;

private:
    using MeanType = double;

    // Decompress: Q = scale * C + bias + Mean
    using ScalarType = DataT; // type for scale and bias
    using Const1Type = DataT; // for l2 distance, const1 = scale * scale * Norm2Sq
    using Const2Type = DataT; // const2 = scale * Norm1

    constexpr static SizeT max_bucket_idx_ = std::numeric_limits<CompressT>::max() - std::numeric_limits<CompressT>::min(); // 255 for i8
    // mean vector is in front
    constexpr static SizeT mean_offset_ = 0;
    // struct for every compress data
    constexpr static SizeT scale_offset_ = 0;
    constexpr static SizeT bias_offset_ = AlignTo(scale_offset_ + sizeof(ScalarType), sizeof(ScalarType));
    constexpr static SizeT const1_offset_ = AlignTo(bias_offset_ + sizeof(ScalarType), sizeof(Const1Type));
    constexpr static SizeT const2_offset_ = AlignTo(const1_offset_ + sizeof(Const1Type), sizeof(Const2Type));
    constexpr static SizeT compress_vec_offset_ = AlignTo(const2_offset_ + sizeof(Const2Type), sizeof(CompressT));

    DataStoreMeta meta_;

    const SizeT compress_data_offset_;
    const SizeT compress_data_size_;

    char *const ptr_;

    const SizeT buffer_plain_size_;
    PlainStore<DataType> plain_data_;

private:
    constexpr MeanType *GetMeanMut() { return reinterpret_cast<MeanType *>(ptr_ + mean_offset_); }

    const char *GetCompressData(SizeT vec_i) const { return ptr_ + compress_data_offset_ + compress_data_size_ * vec_i; }
    char *GetCompressDataMut(SizeT vec_i) { return ptr_ + compress_data_offset_ + compress_data_size_ * vec_i; }

    class LVQDataMut {
        char *const ptr_;

    public:
        LVQDataMut(char *c) : ptr_(c) {}
        Pair<ScalarType *, ScalarType *> GetScalarMut() {
            return {reinterpret_cast<ScalarType *>(ptr_ + scale_offset_), reinterpret_cast<ScalarType *>(ptr_ + bias_offset_)};
        }
        Pair<Const1Type *, Const2Type *> GetConstantMut() {
            return {reinterpret_cast<Const1Type *>(ptr_ + const1_offset_), reinterpret_cast<Const2Type *>(ptr_ + const2_offset_)};
        }
        CompressT *GetCompressVecMut() { return reinterpret_cast<CompressT *>(ptr_ + compress_vec_offset_); }
    };

public:
    constexpr const MeanType *GetMean() const { return reinterpret_cast<const MeanType *>(ptr_ + mean_offset_); }

    class LVQData {
        friend This;
        const char *const ptr_;

    public:
        LVQData(const char *c) : ptr_(c) {}
        Pair<ScalarType, ScalarType> GetScalar() const {
            return {*reinterpret_cast<const ScalarType *>(ptr_ + scale_offset_), *reinterpret_cast<const ScalarType *>(ptr_ + bias_offset_)};
        }
        Pair<Const1Type, Const2Type> GetConstant() const {
            return {*reinterpret_cast<const Const1Type *>(ptr_ + const1_offset_), *reinterpret_cast<const Const2Type *>(ptr_ + const2_offset_)};
        }
        const CompressT *GetCompressVec() const { return reinterpret_cast<const CompressT *>(ptr_ + compress_vec_offset_); }
    };

    SizeT cur_vec_num() const { return meta_.cur_vec_num() + plain_data_.cur_vec_num(); }
    SizeT max_vec_num() const { return meta_.max_vec_num_; }
    SizeT dim() const { return meta_.dim_; }
    static SizeT err_idx() { return DataStoreMeta::ERR_IDX; }

private:
    LVQStore(DataStoreMeta meta, This::InitArgs init_args)
        : meta_(Move(meta)),                                                                                                                  //
          compress_data_offset_(AlignTo(dim() * sizeof(MeanType), init_args.second ? PADDING_SIZE : 1)),                                      //
          compress_data_size_(AlignTo(compress_vec_offset_ + sizeof(CompressT) * dim(), init_args.second ? PADDING_SIZE : 1)),                //
          ptr_(new(std::align_val_t(init_args.second ? PADDING_SIZE : 8)) char[compress_data_offset_ + compress_data_size_ * max_vec_num()]), //
          buffer_plain_size_(init_args.first),                                                                                                //
          plain_data_(PlainStore<DataType>::Make(0, dim()))                                                                                   //
    {
        MeanType *mean = GetMeanMut();
        std::fill(mean, mean + dim(), 0);
    }

public:
    static LVQStore Make(SizeT max_vec_num, SizeT dim, This::InitArgs init_args) {
        DataStoreMeta data_store(max_vec_num, dim);
        return LVQStore(Move(data_store), Move(init_args));
    }

    LVQStore(const LVQStore &) = delete;
    LVQStore &operator=(const LVQStore &) = delete;
    LVQStore &operator=(LVQStore &&other) = delete;

    LVQStore(LVQStore &&other)
        : meta_(Move(other.meta_)),                         //
          compress_data_offset_(other.compress_data_size_), //
          compress_data_size_(other.compress_data_offset_), //
          ptr_(other.ptr_),                                 //
          buffer_plain_size_(other.buffer_plain_size_),     //
          plain_data_(Move(other.plain_data_))              //
    {
        const_cast<char *&>(other.ptr_) = nullptr;
    }

    ~LVQStore() {
        if (ptr_ != nullptr) {
            delete[] ptr_;
        }
    }

private:
    // the caller is responsible for allocate the `result`
    void Decompress(SizeT vec_i, DataType *result) const {
        const MeanType *mean = GetMean();
        LVQData lvq = GetVec(vec_i);
        const CompressT *compress_vec = lvq.GetCompressVec();
        auto [scale, bias] = lvq.GetScalar();
        for (SizeT j = 0; j < dim(); ++j) {
            result[j] = scale * compress_vec[j] + bias + mean[j];
        }
    }

    void CompressVec(const DataType *vec, LVQDataMut &lvq) const {
        const MeanType *mean = GetMean();
        auto [scale_p, bias_p] = lvq.GetScalarMut();
        auto [constant1_p, constant2_p] = lvq.GetConstantMut();
        CompressT *tgt = lvq.GetCompressVecMut();

        ScalarType lower = std::numeric_limits<ScalarType>::max();
        ScalarType upper = -std::numeric_limits<ScalarType>::max();
        for (SizeT j = 0; j < dim(); ++j) {
            auto x = static_cast<ScalarType>(vec[j] - mean[j]);
            lower = std::min(lower, x);
            upper = std::max(upper, x);
        }
        ScalarType scale = (upper - lower) / max_bucket_idx_;
        ScalarType bias = lower - std::numeric_limits<CompressT>::min() * scale;
        i64 norm1 = 0;
        i64 norm2 = 0;
        if (scale == 0) {
            std::fill(tgt, tgt + dim(), 0);
        } else {
            ScalarType scale_inv = 1 / scale;
            for (SizeT j = 0; j < dim(); ++j) {
                auto c = std::floor((vec[j] - mean[j] - bias) * scale_inv + 0.5);
                assert(c <= std::numeric_limits<CompressT>::max() && c >= std::numeric_limits<CompressT>::min());
                tgt[j] = c;
                norm1 += c;
                norm2 += c * c;
            }
        }
        *bias_p = bias;
        *scale_p = scale;
        *constant1_p = norm1 * scale;
        *constant2_p = norm2 * scale * scale;
    }

    SizeT MergeCompress(const DataType *vecs, SizeT vec_num) {
        SizeT compress_n = meta_.cur_vec_num();
        if (auto ret = meta_.AllocateVec(vec_num + plain_data_.cur_vec_num()); ret > max_vec_num()) {
            return err_idx();
        }
        auto decompress_vecs = MakeUnique<DataType[]>(dim() * compress_n);
        for (SizeT vec_i = 0; vec_i < compress_n; ++vec_i) {
            Decompress(vec_i, decompress_vecs.get() + vec_i * dim());
        }
        MeanType *mean = GetMeanMut();
        for (SizeT i = 0; i < dim(); ++i) {
            mean[i] *= compress_n;
        }
        for (SizeT vec_i = 0; vec_i < plain_data_.cur_vec_num(); ++vec_i) {
            const DataType *buffer_vecs = plain_data_.GetVec(vec_i);
            for (SizeT j = 0; j < dim(); ++j) {
                mean[j] += buffer_vecs[j];
            }
        }
        for (SizeT vec_i = 0; vec_i < vec_num; ++vec_i) {
            const DataType *vec = vecs + vec_i * dim();
            for (SizeT j = 0; j < dim(); ++j) {
                mean[j] += vec[j];
            }
        }
        for (SizeT i = 0; i < dim(); ++i) {
            mean[i] /= cur_vec_num();
        }

        for (SizeT vec_i = 0; vec_i < compress_n; ++vec_i) {
            LVQDataMut lvq(GetCompressDataMut(vec_i));
            CompressVec(decompress_vecs.get() + vec_i * dim(), lvq);
        }
        for (SizeT vec_i = compress_n; vec_i < compress_n + plain_data_.cur_vec_num(); ++vec_i) {
            LVQDataMut lvq(GetCompressDataMut(vec_i));
            CompressVec(plain_data_.GetVec(vec_i - compress_n), lvq);
        }
        SizeT start_idx = compress_n + plain_data_.cur_vec_num();
        for (SizeT vec_i = start_idx; vec_i < start_idx + vec_num; ++vec_i) {
            LVQDataMut lvq(GetCompressDataMut(vec_i));
            CompressVec(vecs + (vec_i - start_idx) * dim(), lvq);
        }
        plain_data_ = PlainStore<DataType>::Make(Min(buffer_plain_size_, max_vec_num() - cur_vec_num()), dim());
        return start_idx;
    }

public:
    SizeT AddVec(const DataType *vecs, SizeT vec_num) {
        if (SizeT idx = plain_data_.AddVec(vecs, vec_num); idx != DataStoreMeta::ERR_IDX) {
            return meta_.cur_vec_num() + idx;
        } else if (SizeT idx = MergeCompress(vecs, vec_num); idx != DataStoreMeta::ERR_IDX) {
            return idx;
        } else {
            return err_idx();
        }
    }

    RtnType GetVec(SizeT vec_i) const {
        assert(vec_i < meta_.cur_vec_num()); // must call compress before get vec
        return LVQData(GetCompressData(vec_i));
    }

    struct QueryCtx {
        const char *compress_query_;
    };

    QueryCtx MakeCtx(const DataType *vecs) const {
        char *compress_query_ = new (std::align_val_t(PADDING_SIZE)) char[compress_vec_offset_ + sizeof(CompressT) * dim()];
        LVQDataMut lvq(compress_query_);
        CompressVec(vecs, lvq);
        return QueryCtx{compress_query_};
    }

    RtnType GetVec(const QueryCtx &query_ctx) const { return LVQData(query_ctx.compress_query_); }

    void Compress() {
        SizeT ret = MergeCompress(nullptr, 0);
        assert(ret != DataStoreMeta::ERR_IDX);
    }

    void Save(FileHandler &file_handler) {
        Compress();
        meta_.Save(file_handler);
        file_handler.Write(ptr_, compress_data_offset_ + compress_data_size_ * cur_vec_num());
    }

    static LVQStore Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs init_args) {
        DataStoreMeta meta = DataStoreMeta::Load(file_handler, max_vec_num);
        LVQStore ret(Move(meta), Move(init_args));
        file_handler.Read(ret.ptr_, ret.compress_data_offset_ + ret.compress_data_size_ * ret.cur_vec_num());
        return ret;
    }

    void Prefetch(SizeT vec_i) const {
        if (vec_i < meta_.cur_vec_num()) {
            _mm_prefetch(GetVec(vec_i).ptr_, _MM_HINT_T0);
        } else {
            plain_data_.Prefetch(vec_i - meta_.cur_vec_num());
        }
    }
};

} // namespace infinity