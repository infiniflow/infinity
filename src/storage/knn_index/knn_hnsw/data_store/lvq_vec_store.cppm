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

#include <cassert>
#include <ostream>

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module lvq_vec_store;

import stl;
import file_system;
import hnsw_common;

namespace infinity {

export template <typename DataType, typename LocalCacheType, typename CompressType>
struct LVQData {
    DataType scale_;
    DataType bias_;
    LocalCacheType local_cache_;
    CompressType compress_vec_[];
};

export template <typename DataType, typename CompressType, typename LVQCache>
class LVQVecStoreInner;

export template <typename DataType, typename CompressType, typename LVQCache>
class LVQVecStoreMeta {
public:
    // Compress type must be i8 temporarily
    static_assert(std::is_same<CompressType, i8>() || std::is_same<CompressType, void>());
    constexpr static SizeT max_bucket_idx_ = std::numeric_limits<CompressType>::max() - std::numeric_limits<CompressType>::min(); // 255 for i8

    using This = LVQVecStoreMeta<DataType, CompressType, LVQCache>;
    using Inner = LVQVecStoreInner<DataType, CompressType, LVQCache>;
    using LocalCacheType = LVQCache::LocalCacheType;
    using GlobalCacheType = LVQCache::GlobalCacheType;
    using LVQData = LVQData<DataType, LocalCacheType, CompressType>;
    using StoreType = const LVQData *;
    struct LVQQuery {
        UniquePtr<LVQData> inner_;
        operator const LVQData *() const { return inner_.get(); }

        LVQQuery(SizeT compress_data_size) : inner_(new(new char[compress_data_size]) LVQData) {}
        LVQQuery(LVQQuery &&other) = default;
        ~LVQQuery() { delete[] reinterpret_cast<char *>(inner_.release()); }
    };
    using QueryType = LVQQuery;

private:
    LVQVecStoreMeta(SizeT dim) : dim_(dim), compress_data_size_(sizeof(LVQData) + sizeof(CompressType) * dim) {
        mean_ = MakeUnique<MeanType[]>(dim);
        std::fill(mean_.get(), mean_.get() + dim, 0);
        global_cache_ = LVQCache::MakeGlobalCache(mean_.get(), dim);
    }

public:
    LVQVecStoreMeta() : dim_(0), compress_data_size_(0), normalize_(false) {}
    LVQVecStoreMeta(This &&other)
        : dim_(std::exchange(other.dim_, 0)), compress_data_size_(std::exchange(other.compress_data_size_, 0)), mean_(std::move(other.mean_)),
          global_cache_(std::exchange(other.global_cache_, GlobalCacheType())), normalize_(other.normalize_) {}
    LVQVecStoreMeta &operator=(This &&other) {
        if (this != &other) {
            dim_ = std::exchange(other.dim_, 0);
            compress_data_size_ = std::exchange(other.compress_data_size_, 0);
            mean_ = std::move(other.mean_);
            global_cache_ = std::exchange(other.global_cache_, GlobalCacheType());
            normalize_ = other.normalize_;
        }
        return *this;
    }

    static This Make(SizeT dim) { return This(dim); }
    static This Make(SizeT dim, bool normalize) {
        This ret(dim);
        ret.normalize_ = normalize;
        return ret;
    }

    void Save(FileHandler &file_handler) const {
        file_handler.Write(&dim_, sizeof(dim_));
        file_handler.Write(mean_.get(), sizeof(MeanType) * dim_);
        file_handler.Write(&global_cache_, sizeof(GlobalCacheType));
    }

    static This Load(FileHandler &file_handler) {
        SizeT dim;
        file_handler.Read(&dim, sizeof(dim));
        This meta(dim);
        file_handler.Read(meta.mean_.get(), sizeof(MeanType) * dim);
        file_handler.Read(&meta.global_cache_, sizeof(GlobalCacheType));
        return meta;
    }

    LVQQuery MakeQuery(const DataType *vec) const {
        LVQQuery query(compress_data_size_);
        CompressTo(vec, query.inner_.get());
        return query;
    }

    void CompressTo(const DataType *src, LVQData *dest) const {
        if (normalize_) {
            DataType norm = 0;
            DataType *src_without_const = const_cast<DataType *>(src);
            for (SizeT j = 0; j < this->dim_; ++j) {
                norm += src_without_const[j] * src_without_const[j];
            }
            norm = std::sqrt(norm);
            if (norm == 0) {
                std::fill(dest->compress_vec_, dest->compress_vec_ + this->dim_, 0);
            } else {
                for (SizeT j = 0; j < this->dim_; ++j) {
                    src_without_const[j] /= norm;
                }
            }
        }

        CompressType *compress = dest->compress_vec_;

        DataType lower = std::numeric_limits<DataType>::max();
        DataType upper = -std::numeric_limits<DataType>::max();
        for (SizeT j = 0; j < dim_; ++j) {
            auto x = static_cast<DataType>(src[j] - mean_[j]);
            lower = std::min(lower, x);
            upper = std::max(upper, x);
        }
        DataType scale = (upper - lower) / max_bucket_idx_;
        DataType bias = lower - std::numeric_limits<CompressType>::min() * scale;
        if (scale == 0) {
            std::fill(compress, compress + dim_, 0);
        } else {
            DataType scale_inv = 1 / scale;
            for (SizeT j = 0; j < dim_; ++j) {
                auto c = std::floor((src[j] - mean_[j] - bias) * scale_inv + 0.5);
                assert(c <= std::numeric_limits<CompressType>::max() && c >= std::numeric_limits<CompressType>::min());
                compress[j] = c;
            }
        }
        dest->scale_ = scale;
        dest->bias_ = bias;
        dest->local_cache_ = LVQCache::MakeLocalCache(compress, scale, dim_, mean_.get());
    }

    template <typename LabelType, DataIteratorConcept<const DataType *, LabelType> Iterator>
    void Optimize(Iterator &&query_iter, const Vector<Pair<Inner *, SizeT>> &inners) {
        auto new_mean = MakeUnique<MeanType[]>(dim_);
        auto temp_decompress = MakeUnique<DataType[]>(dim_);
        SizeT cur_vec_num = 0;
        for (const auto [inner, size] : inners) {
            for (SizeT i = 0; i < size; ++i) {
                DecompressTo(inner->GetVec(i, *this), temp_decompress.get());
                for (SizeT j = 0; j < dim_; ++j) {
                    new_mean[j] += temp_decompress[j];
                }
            }
            cur_vec_num += size;
        }
        while (true) {
            if (auto ret = query_iter.Next(); ret) {
                auto &[vec, _] = *ret;
                for (SizeT i = 0; i < dim_; ++i) {
                    new_mean[i] += vec[i];
                }
                ++cur_vec_num;
            } else {
                break;
            }
        }
        for (SizeT i = 0; i < dim_; ++i) {
            new_mean[i] /= cur_vec_num;
        }
        swap(new_mean, mean_);

        for (auto [inner, size] : inners) {
            for (SizeT i = 0; i < size; ++i) {
                DecompressByMeanTo(inner->GetVec(i, *this), new_mean.get(), temp_decompress.get());
                inner->SetVec(i, temp_decompress.get(), *this);
            }
        }
        global_cache_ = LVQCache::MakeGlobalCache(mean_.get(), dim_);
    }

    SizeT dim() const { return dim_; }
    SizeT compress_data_size() const { return compress_data_size_; }

    const GlobalCacheType &global_cache() const { return global_cache_; }

    // for unit test
    const MeanType *mean() const { return mean_.get(); }

private:
    void DecompressByMeanTo(const LVQData *src, const MeanType *mean, DataType *dest) const {
        const CompressType *compress = src->compress_vec_;
        DataType scale = src->scale_;
        DataType bias = src->bias_;
        for (SizeT i = 0; i < dim_; ++i) {
            dest[i] = scale * compress[i] + bias + mean[i];
        }
    }

    void DecompressTo(const LVQData *src, DataType *dest) const { DecompressByMeanTo(src, mean_.get(), dest); };

private:
    SizeT dim_;
    SizeT compress_data_size_;

    UniquePtr<MeanType[]> mean_;
    GlobalCacheType global_cache_;

    bool normalize_{false};

public:
    void Dump(std::ostream &os) const {
        os << "[CONST] dim: " << dim_ << ", compress_data_size: " << compress_data_size_ << std::endl;
        os << "mean: ";
        for (SizeT i = 0; i < dim_; ++i) {
            os << mean_[i] << " ";
        }
        os << std::endl;
        LVQCache::DumpGlobalCache(os, global_cache_);
    }
};

export template <typename DataType, typename CompressType, typename LVQCache>
class LVQVecStoreInner {
public:
    using This = LVQVecStoreInner<DataType, CompressType, LVQCache>;
    using Meta = LVQVecStoreMeta<DataType, CompressType, LVQCache>;
    // Decompress: Q = scale * C + bias + Mean
    using LocalCacheType = LVQCache::LocalCacheType;
    using LVQData = LVQData<DataType, LocalCacheType, CompressType>;

private:
    LVQVecStoreInner(SizeT max_vec_num, const Meta &meta) : ptr_(MakeUnique<char[]>(max_vec_num * meta.compress_data_size())) {}

public:
    LVQVecStoreInner() = default;

    static This Make(SizeT max_vec_num, const Meta &meta) { return This(max_vec_num, meta); }

    void Save(FileHandler &file_handler, SizeT cur_vec_num, const Meta &meta) const {
        file_handler.Write(ptr_.get(), cur_vec_num * meta.compress_data_size());
    }

    static This Load(FileHandler &file_handler, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta) {
        assert(cur_vec_num <= max_vec_num);
        This ret(max_vec_num, meta);
        file_handler.Read(ret.ptr_.get(), cur_vec_num * meta.compress_data_size());
        return ret;
    }

    void SetVec(SizeT idx, const DataType *vec, const Meta &meta) { meta.CompressTo(vec, GetVecMut(idx, meta)); }

    const LVQData *GetVec(SizeT idx, const Meta &meta) const {
        return reinterpret_cast<const LVQData *>(ptr_.get() + idx * meta.compress_data_size());
    }

    void Prefetch(VertexType vec_i, const Meta &meta) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i, meta)), _MM_HINT_T0); }

private:
    LVQData *GetVecMut(SizeT idx, const Meta &meta) { return reinterpret_cast<LVQData *>(ptr_.get() + idx * meta.compress_data_size()); }

private:
    UniquePtr<char[]> ptr_;

public:
    void Dump(std::ostream &os, SizeT offset, SizeT chunk_size, const Meta &meta) const {
        for (int i = 0; i < (int)chunk_size; ++i) {
            os << "vec " << i << "(" << offset + i << "): ";
            const LVQData *vec = GetVec(i, meta);
            os << "scale: " << vec->scale_ << ", bias: " << vec->bias_ << std::endl;
            os << "compress_vec: ";
            for (SizeT j = 0; j < meta.dim(); ++j) {
                os << static_cast<int>(vec->compress_vec_[j]) << " ";
            }
            os << std::endl;
            LVQCache::DumpLocalCache(os, vec->local_cache_);
        }
    }
};

} // namespace infinity