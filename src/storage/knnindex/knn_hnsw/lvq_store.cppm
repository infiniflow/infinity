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
#include <cmath>
#include <new>
#include <type_traits>
#include <xmmintrin.h>

import stl;
import hnsw_common;
import file_system;
import plain_store;
import infinity_exception;

export module lvq_store;

namespace infinity {

export template <typename DataType, typename CompressType, LVQCacheConcept<DataType, CompressType> LVQCache>
class LVQStore {
public:
    // Compress type must be i8 temporarily
    static_assert(std::is_same<CompressType, i8>());

    using This = LVQStore<DataType, CompressType, LVQCache>;
    using InitArgs = SizeT; // buffer size
    class LVQData;
    using StoreType = LVQData;
    struct QueryLVQ;
    using QueryType = QueryLVQ;

    static constexpr SizeT ERR_IDX = DataStoreMeta::ERR_IDX;
    static constexpr SizeT PADDING_SIZE = 32;

private:
    constexpr static SizeT max_bucket_idx_ = LimitMax<CompressType>() - LimitMin<CompressType>(); // 255 for i8

    // Decompress: Q = scale * C + bias + Mean
    using ScalarType = DataType; // type for scale and bias

    using LocalCacheType = LVQCache::LocalCacheType;
    using GlobalCacheType = LVQCache::GlobalCacheType;

    // mean vector is in front
    constexpr static SizeT global_cache_offset_ = 0;
    constexpr static SizeT mean_offset_ = AlignTo(global_cache_offset_ + sizeof(GlobalCacheType), sizeof(MeanType));
    // struct for every compress data
    constexpr static SizeT scale_offset_ = 0;
    constexpr static SizeT bias_offset_ = AlignTo(scale_offset_ + sizeof(ScalarType), sizeof(ScalarType));
    constexpr static SizeT local_cache_offset_ = AlignTo(bias_offset_ + sizeof(ScalarType), sizeof(LocalCacheType));
    constexpr static SizeT compress_vec_offset_ = AlignTo(local_cache_offset_ + sizeof(LocalCacheType), sizeof(CompressType));

    DataStoreMeta meta_;

    const SizeT compress_data_offset_;
    const SizeT compress_data_size_;

    char *const ptr_;

    const SizeT buffer_plain_size_;
    PlainStore<DataType> plain_data_;

private:
    constexpr GlobalCacheType *GetGlobalCacheMut() { return reinterpret_cast<GlobalCacheType *>(ptr_ + global_cache_offset_); }
    constexpr MeanType *GetMeanMut() { return reinterpret_cast<MeanType *>(ptr_ + mean_offset_); }

    LVQData GetLVQData(SizeT vec_i) const { return LVQData(ptr_ + compress_data_offset_ + compress_data_size_ * vec_i); }

public:
    constexpr const GlobalCacheType GetGlobalCache() const { return *reinterpret_cast<const GlobalCacheType *>(ptr_ + global_cache_offset_); }
    constexpr const MeanType *GetMean() const { return reinterpret_cast<const MeanType *>(ptr_ + mean_offset_); }

    struct QueryLVQ {
        const UniquePtr<char[]> ptr_;
    };

    class LVQData {
        friend This;
        char *const ptr_;

        Pair<ScalarType *, ScalarType *> GetScalarMut() const {
            return {reinterpret_cast<ScalarType *>(ptr_ + scale_offset_), reinterpret_cast<ScalarType *>(ptr_ + bias_offset_)};
        }
        LocalCacheType *GetLocalCacheMut() const { return reinterpret_cast<LocalCacheType *>(ptr_ + local_cache_offset_); }
        CompressType *GetCompressVecMut() const { return reinterpret_cast<CompressType *>(ptr_ + compress_vec_offset_); }

    public:
        LVQData(char *c) : ptr_(c) {}

        LVQData(const QueryLVQ &query) : ptr_(query.ptr_.get()) {}

        Pair<ScalarType, ScalarType> GetScalar() const {
            return {*reinterpret_cast<const ScalarType *>(ptr_ + scale_offset_), *reinterpret_cast<const ScalarType *>(ptr_ + bias_offset_)};
        }
        LocalCacheType GetLocalCache() const { return *reinterpret_cast<const LocalCacheType *>(ptr_ + local_cache_offset_); }
        const CompressType *GetCompressVec() const { return reinterpret_cast<const CompressType *>(ptr_ + compress_vec_offset_); }
    };

    SizeT cur_vec_num() const { return meta_.cur_vec_num() + plain_data_.cur_vec_num(); }
    SizeT max_vec_num() const { return meta_.max_vec_num_; }
    SizeT dim() const { return meta_.dim_; }

private:
    LVQStore(DataStoreMeta meta, This::InitArgs init_args)
        : meta_(Move(meta)),                                                                                                                      //
          compress_data_offset_(AlignTo(mean_offset_ + dim() * sizeof(MeanType), PADDING_SIZE)),                                                  //
          compress_data_size_(AlignTo(compress_vec_offset_ + sizeof(CompressType) * dim(), PADDING_SIZE)),                                        //
          ptr_(static_cast<char *>(operator new[](compress_data_offset_ + compress_data_size_ * max_vec_num(), std::align_val_t(PADDING_SIZE)))), //
          buffer_plain_size_(init_args),                                                                                                          //
          plain_data_(PlainStore<DataType>::Make(0, dim()))                                                                                       //
    {}

    void Init() {
        // MeanType *mean = GetMeanMut();
        // Fill(mean, mean + dim(), 0);
        Fill(ptr_, ptr_ + compress_data_offset_ + compress_data_size_ * max_vec_num(), 0);
    }

public:
    static This Make(SizeT max_vec_num, SizeT dim, This::InitArgs init_args) {
        DataStoreMeta meta(max_vec_num, dim);
        auto ret = This(Move(meta), Move(init_args));
        ret.Init();
        return ret;
    }

    LVQStore(const This &) = delete;
    LVQStore &operator=(const This &) = delete;
    LVQStore &operator=(This &&other) = delete;

    LVQStore(This &&other)
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
            operator delete[](ptr_, std::align_val_t(PADDING_SIZE));
        }
    }

    void Save(FileHandler &file_handler) {
        Compress();
        meta_.Save(file_handler);
        file_handler.Write(ptr_, compress_data_offset_ + compress_data_size_ * cur_vec_num());
    }

    static This Load(FileHandler &file_handler, SizeT max_vec_num, This::InitArgs init_args) {
        DataStoreMeta meta = DataStoreMeta::Load(file_handler, max_vec_num);
        auto ret = This(Move(meta), Move(init_args));
        file_handler.Read(ret.ptr_, ret.compress_data_offset_ + ret.compress_data_size_ * ret.cur_vec_num());
        return ret;
    }

private:
    // the caller is responsible for allocate the `result`
    void Decompress(SizeT vec_i, DataType *result) const {
        const MeanType *mean = GetMean();
        LVQData lvq = GetVec(vec_i);
        const CompressType *compress_vec = lvq.GetCompressVec();
        auto [scale, bias] = lvq.GetScalar();
        for (SizeT j = 0; j < dim(); ++j) {
            result[j] = scale * compress_vec[j] + bias + mean[j];
        }
    }

    void CompressVec(const DataType *vec, const LVQData &lvq) const {
        const MeanType *mean = GetMean();
        auto [scale_p, bias_p] = lvq.GetScalarMut();
        CompressType *compress = lvq.GetCompressVecMut();

        ScalarType lower = LimitMax<ScalarType>();
        ScalarType upper = -LimitMax<ScalarType>();
        for (SizeT j = 0; j < dim(); ++j) {
            auto x = static_cast<ScalarType>(vec[j] - mean[j]);
            lower = Min(lower, x);
            upper = Max(upper, x);
        }
        ScalarType scale = (upper - lower) / max_bucket_idx_;
        ScalarType bias = lower - LimitMin<CompressType>() * scale;
        if (scale == 0) {
            Fill(compress, compress + dim(), 0);
        } else {
            ScalarType scale_inv = 1 / scale;
            for (SizeT j = 0; j < dim(); ++j) {
                auto c = std::floor((vec[j] - mean[j] - bias) * scale_inv + 0.5);
                if(!(c <= LimitMax<CompressType>() && c >= LimitMin<CompressType>())) {
                    Error<StorageException>("CompressVec error");
                }
                compress[j] = c;
            }
        }
        *bias_p = bias;
        *scale_p = scale;
        *lvq.GetLocalCacheMut() = LVQCache::MakeLocalCache(compress, scale, dim(), mean);
    }

    // TODO SIMD optimization here
    template <typename Iterator>
        requires DataIteratorConcept<Iterator, const DataType *>
    SizeT MergeCompress(Iterator query_iter, SizeT vec_num) {
        SizeT compress_n = meta_.cur_vec_num();
        if (auto ret = meta_.AllocateVec(vec_num + plain_data_.cur_vec_num()); ret > max_vec_num()) {
            return ERR_IDX;
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
        Iterator query_iter1 = query_iter;

        for (SizeT i = 0; i < vec_num; ++i) {
            auto vec = *(query_iter1.Next());
            for (SizeT j = 0; j < dim(); ++j) {
                mean[j] += vec[j];
            }
        }
        for (SizeT i = 0; i < dim(); ++i) {
            mean[i] /= cur_vec_num();
        }

        SizeT vec_i = 0;
        while (vec_i < compress_n) {
            CompressVec(decompress_vecs.get() + vec_i * dim(), GetLVQData(vec_i));
            ++vec_i;
        }
        while (vec_i < compress_n + plain_data_.cur_vec_num()) {
            CompressVec(plain_data_.GetVec(vec_i - compress_n), GetLVQData(vec_i));
            ++vec_i;
        }
        while (vec_i < compress_n + plain_data_.cur_vec_num() + vec_num) {
            auto vec = *(query_iter.Next());
            CompressVec(vec, GetLVQData(vec_i));
            ++vec_i;
        }
        *GetGlobalCacheMut() = LVQCache::MakeGlobalCache(GetMean(), dim());
        plain_data_ = PlainStore<DataType>::Make(Min(buffer_plain_size_, max_vec_num() - cur_vec_num()), dim());
        return cur_vec_num() - vec_num;
    }

public:
    SizeT AddVec(const DataType *vec, SizeT vec_num) { return AddVec(DenseVectorIter(vec, dim(), vec_num), vec_num); }

    template <typename Iterator>
        requires DataIteratorConcept<Iterator, const DataType *>
    SizeT AddVec(Iterator query_iter, SizeT vec_num) {
        if (SizeT idx = plain_data_.AddVec(query_iter, vec_num); idx != DataStoreMeta::ERR_IDX) {
            return meta_.cur_vec_num() + idx;
        } else if (SizeT idx = MergeCompress(Move(query_iter), vec_num); idx != DataStoreMeta::ERR_IDX) {
            return idx;
        } else {
            return ERR_IDX;
        }
    }

    StoreType GetVec(SizeT vec_i) const {
        if(vec_i >= meta_.cur_vec_num()) {
            Error<StorageException>("Get Vec error");
        }
        return GetLVQData(vec_i);
    }

    void Prefetch(SizeT vec_i) const {
        if (vec_i < meta_.cur_vec_num()) {
            _mm_prefetch(GetVec(vec_i).ptr_, _MM_HINT_T0);
        } else {
            plain_data_.Prefetch(vec_i - meta_.cur_vec_num());
        }
    }

    QueryType MakeQuery(const DataType *vec) const {
        auto ptr = MakeUnique<char[]>(compress_data_size_);
        CompressVec(vec, LVQData(ptr.get()));
        return QueryType{Move(ptr)};
    }

    void Compress() {
        // query_iter is empty here. Should implement better
        DenseVectorIter<DataType> empty_iter(nullptr, dim(), 0);
        SizeT ret = MergeCompress(empty_iter, 0);
        if(ret == DataStoreMeta::ERR_IDX) {
            Error<StorageException>("Compress error");
        }
    }
};

} // namespace infinity