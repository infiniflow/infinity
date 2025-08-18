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
#include <random>

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module infinity_core:rabitq_vec_store;

import :stl;
import :local_file_handle;
import :data_store_util;
import :infinity_exception;
import :hnsw_common;
import serialize;

namespace infinity {

SizeT AlignUp(SizeT num, SizeT align_size) { return (num + align_size - 1) & ~(align_size - 1); }

template <typename DataType>
void GenerateRandomOrthogonalMatrix(DataType *rom, SizeT dim) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<DataType> dist(-1.0, 1.0);

    // generate random matrix
    for (SizeT i = 0; i < dim; ++i)
        for (SizeT j = 0; j < dim; ++j)
            rom[i * dim + j] = dist(gen);

    // TODO: Achieved through QR decomposition
    // Gram-Schmidt orthogonalization
    for (SizeT j = 0; j < dim; ++j) {
        // Orthogonalize column vector
        for (SizeT k = 0; k < j; ++k) {
            double dot = 0.0;
            for (SizeT i = 0; i < dim; ++i)
                dot += rom[i * dim + k] * rom[i * dim + j];
            for (SizeT i = 0; i < dim; ++i)
                rom[i * dim + j] -= dot * rom[i * dim + k];
        }

        // Normalized column vector
        double norm = 0.0;
        for (SizeT i = 0; i < dim; ++i)
            norm += rom[i * dim + j] * rom[i * dim + j];
        norm = sqrt(norm);
        for (SizeT i = 0; i < dim; ++i)
            rom[i * dim + j] /= norm;
    }
}

/////////////////////////// compress data content ///////////////////////////

export template <typename DataType, typename AlignType>
struct RabitqStoreData {
    DataType norm_{0};
    DataType sum_{0};
    DataType raw_norm_{0};
    DataType error_{0};
    AlignType compress_vec_[];
};

export template <typename DataType, typename AlignType>
struct RabitqQueryData {
    DataType query_norm_{0};
    DataType query_sum_{0};
    DataType query_raw_norm_{0};
    DataType query_lower_bound_{0};
    DataType query_delta_{0};
    AlignType compress_vec_[];
};

/////////////////////////// meta info of rabitq ///////////////////////////

export template <typename DataType, bool OwnMem>
class RabitqVecStoreInner;

export template <typename DataType>
class RabitqVecStoreMetaType {
public:
    using AlignType = i8;
    using DistanceType = f32;
    using StoreData = RabitqStoreData<DataType, AlignType>;
    using QueryData = RabitqQueryData<DataType, AlignType>;
    using StoreType = const StoreData *;
    struct QueryType {
        UniquePtr<QueryData> inner_;
        operator const QueryData *() const { return inner_.get(); }

        QueryType(SizeT compress_data_size) : inner_(new(new char[compress_data_size]) QueryData) {}
        QueryType(QueryType &&other) = default;
        ~QueryType() { delete[] reinterpret_cast<char *>(inner_.release()); }
    };
};

template <typename DataType, bool OwnMem>
class RabitqVecStoreMetaBase {
public:
    // DataType type must be i8 & float temporarily
    static_assert(std::is_same<DataType, i8>() || std::is_same<DataType, float>());

    using This = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using Inner = RabitqVecStoreInner<DataType, OwnMem>;
    using MetaType = RabitqVecStoreMetaType<DataType>;
    using StoreData = typename MetaType::StoreData;
    using QueryData = typename MetaType::QueryData;
    using StoreType = typename MetaType::StoreType;
    using QueryType = typename MetaType::QueryType;
    using AlignType = typename MetaType::AlignType;
    using DistanceType = typename MetaType::DistanceType;

    constexpr static SizeT align_size_ = sizeof(AlignType) * 8;                                                             // 8 for i8
    constexpr static SizeT max_bucket_idx_ = std::numeric_limits<AlignType>::max() - std::numeric_limits<AlignType>::min(); // 255 for i8

public:
    RabitqVecStoreMetaBase() : align_dim_(0), compress_data_size_(0) {}
    RabitqVecStoreMetaBase(This &&other)
        : align_dim_(std::exchange(other.align_dim_, 0)), compress_data_size_(std::exchange(other.compress_data_size_, 0)),
          rom_(std::exchange(other.rom_)) {}
    RabitqVecStoreMetaBase &operator=(This &&other) {
        if (this != &other) {
            align_dim_ = std::exchange(other.align_dim_, 0);
            compress_data_size_ = std::exchange(other.compress_data_size_, 0);
            rom_ = std::move(other.rom_);
        }
        return *this;
    }

    void Save(LocalFileHandle &file_handle) const {
        file_handle.Append(&align_dim_, sizeof(align_dim_));
        file_handle.Append(rom_.get(), sizeof(MeanType) * align_dim_ * align_dim_);
    }

    QueryType MakeQuery(const DataType *vec) const {
        QueryType query(compress_data_size_);
        CompressToQuery(vec, query.inner_.get());
        return query;
    }

    void CompressToCode(const DataType *src, StoreData *dest) const { UnrecoverableError("Not implemented"); }

    void CompressToQuery(const DataType *src, QueryData *dest) const { UnrecoverableError("Not implemented"); }

    void Dump(std::ostream &os) const {
        os << "[CONST] dim: " << align_dim_ << ", compress_data_size: " << compress_data_size_ << std::endl;
        os << "rom: " << std::endl;
        for (SizeT i = 0; i < align_dim_; ++i) {
            for (SizeT j = 0; j < align_dim_; ++j) {
                os << rom_[i * align_dim_ + j] << " ";
            }
            os << std::endl;
        }
    }

public:
    SizeT GetSizeInBytes() const { return sizeof(align_dim_); }
    SizeT GetVecSizeInBytes() const { return compress_data_size_; }
    SizeT dim() const { return align_dim_; }
    SizeT compress_data_size() const { return compress_data_size_; }
    DataType *rom() const { return rom_.get(); }

protected:
    SizeT align_dim_;
    SizeT compress_data_size_;
    ArrayPtr<DataType, OwnMem> rom_; // Random orthogonal matrix
};

export template <typename DataType, bool OwnMem>
class RabitqVecStoreMeta : public RabitqVecStoreMetaBase<DataType, OwnMem> {
public:
    using This = RabitqVecStoreMeta<DataType, OwnMem>;
    using Base = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using Inner = RabitqVecStoreInner<DataType, OwnMem>;
    using StoreData = typename Base::StoreData;
    using AlignType = typename Base::AlignType;

private:
    RabitqVecStoreMeta(SizeT dim) {
        SizeT align_dim = AlignUp(dim, Base::align_size_);
        this->align_dim_ = align_dim;
        this->compress_data_size_ = sizeof(StoreData) + align_dim / 8;
        this->rom_ = MakeUnique<DataType[]>(align_dim * align_dim);
        GenerateRandomOrthogonalMatrix<DataType>(this->rom_.get(), align_dim);
    }

public:
    RabitqVecStoreMeta() = delete;
    static This Make(SizeT dim) { return This(dim); }

    static This Load(LocalFileHandle &file_handle) {
        SizeT dim;
        file_handle.Read(&dim, sizeof(dim));
        return This(dim);
    }

    static This LoadFromPtr(const char *&ptr) {
        SizeT dim = ReadBufAdv<SizeT>(ptr);
        return This(dim);
    }

    template <typename LabelType, DataIteratorConcept<const DataType *, LabelType> Iterator>
    void Optimize(Iterator &&query_iter, const Vector<Pair<Inner *, SizeT>> &inners, SizeT &mem_usage) {
        UnrecoverableError("Not implemented");
    }
};

export template <typename DataType>
class RabitqVecStoreMeta<DataType, false> : public RabitqVecStoreMetaBase<DataType, false> {
public:
    using This = RabitqVecStoreMeta<DataType, false>;
    using Base = RabitqVecStoreMetaBase<DataType, false>;
    using StoreData = typename Base::StoreData;
    using AlignType = typename Base::AlignType;

private:
    RabitqVecStoreMeta(SizeT dim) {
        SizeT align_dim = AlignUp(dim, Base::align_size_);
        this->align_dim_ = align_dim;
        this->compress_data_size_ = sizeof(StoreData) + align_dim / 8;
        this->rom_ = MakeUnique<DataType[]>(align_dim * align_dim);
        GenerateRandomOrthogonalMatrix<DataType>(this->rom_.get(), align_dim);
    }

public:
    RabitqVecStoreMeta() = delete;

    static This LoadFromPtr(const char *&ptr) {
        SizeT dim = ReadBufAdv<SizeT>(ptr);
        return This(dim);
    }
};

/////////////////////////// data operation of inner ///////////////////////////

template <typename DataType, bool OwnMem>
class RabitqVecStoreInnerBase {
public:
    using This = RabitqVecStoreInnerBase<DataType, OwnMem>;
    using Meta = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using StoreType = typename Meta::StoreType;

public:
    RabitqVecStoreInnerBase() = default;

    SizeT GetSizeInBytes(SizeT cur_vec_num, const Meta &meta) const { return cur_vec_num * meta.compress_data_size(); }

    void Save(LocalFileHandle &file_handle, SizeT cur_vec_num, const Meta &meta) const {
        file_handle.Append(ptr_.get(), cur_vec_num * meta.compress_data_size());
    }

    static void SaveToPtr(LocalFileHandle &file_handle,
                          const Vector<const This *> &inners,
                          const Meta &meta,
                          SizeT ck_size,
                          SizeT chunk_num,
                          SizeT last_chunk_size) {
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            file_handle.Append(inners[i]->ptr_.get(), chunk_size * meta.compress_data_size());
        }
    }

    StoreType GetVec(SizeT idx, const Meta &meta) const { return reinterpret_cast<StoreType>(ptr_.get() + idx * meta.compress_data_size()); }

    void Prefetch(VertexType vec_i, const Meta &meta) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i, meta)), _MM_HINT_T0); }

protected:
    ArrayPtr<char, OwnMem> ptr_;
};

export template <typename DataType, bool OwnMem>
class RabitqVecStoreInner : public RabitqVecStoreInnerBase<DataType, OwnMem> {
public:
    using This = RabitqVecStoreInner<DataType, OwnMem>;
    using Meta = RabitqVecStoreMetaBase<DataType, OwnMem>;
    using StoreData = typename Meta::StoreData;

private:
    RabitqVecStoreInner(SizeT max_vec_num, const Meta &meta) { this->ptr_ = MakeUnique<char[]>(max_vec_num * meta.compress_data_size()); }

public:
    RabitqVecStoreInner() = delete;

    static This Make(SizeT max_vec_num, const Meta &meta, SizeT &mem_usage) {
        auto ret = This(max_vec_num, meta);
        mem_usage += max_vec_num * meta.compress_data_size();
        return ret;
    }

    static This Load(LocalFileHandle &file_handle, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta, SizeT &mem_usage) {
        assert(cur_vec_num <= max_vec_num);
        This ret(max_vec_num, meta);
        file_handle.Read(ret.ptr_.get(), cur_vec_num * meta.compress_data_size());
        mem_usage += max_vec_num * meta.compress_data_size();
        return ret;
    }

    static This LoadFromPtr(const char *&ptr, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta, SizeT &mem_usage) {
        This ret(max_vec_num, meta);
        std::memcpy(ret.ptr_.get(), ptr, cur_vec_num * meta.compress_data_size());
        ptr += cur_vec_num * meta.compress_data_size();
        mem_usage += max_vec_num * meta.compress_data_size();
        return ret;
    }

    void SetVec(SizeT idx, const DataType *vec, const Meta &meta, SizeT &mem_usage) { meta.CompressToCode(vec, GetVecMut(idx, meta)); }

private:
    StoreData *GetVecMut(SizeT idx, const Meta &meta) { return reinterpret_cast<StoreData *>(this->ptr_.get() + idx * meta.compress_data_size()); }
};

export template <typename DataType>
class RabitqVecStoreInner<DataType, false> : public RabitqVecStoreInnerBase<DataType, false> {
public:
    using This = RabitqVecStoreInner<DataType, false>;
    using Meta = RabitqVecStoreMetaBase<DataType, false>;

private:
    RabitqVecStoreInner(const char *ptr) { this->ptr_ = ptr; }

public:
    RabitqVecStoreInner() = delete;

    static This LoadFromPtr(const char *&ptr, SizeT cur_vec_num, const Meta &meta) {
        const char *p = ptr;
        This ret(p);
        ptr += cur_vec_num * meta.compress_data_size();
        return ret;
    }
};

} // namespace infinity