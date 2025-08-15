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

#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module infinity_core:plain_vec_store;

import :stl;
import :local_file_handle;
import :hnsw_common;
import :data_store_util;

import std;

import data_type;
import serialize;

namespace infinity {

export template <typename DataType>
class PlainVecStoreMeta {
public:
    using This = PlainVecStoreMeta<DataType>;
    using StoreType = const DataType *;
    using QueryType = const DataType *;
    using DistanceType = f32;

private:
    PlainVecStoreMeta(size_t dim) : dim_(dim) {}

public:
    PlainVecStoreMeta() : dim_(0) {}
    // PlainVecStoreMeta(This &&other) : dim_(std::exchange(other.dim_, 0)) {}
    // PlainVecStoreMeta &operator=(This &&other) {
    //     if (this != &other) {
    //         dim_ = std::exchange(other.dim_, 0);
    //     }
    //     return *this;
    // }

    static This Make(size_t dim) { return This(dim); }
    static This Make(size_t dim, bool) { return This(dim); }

    size_t GetSizeInBytes() const { return sizeof(size_t); }

    // Get size of vector in search
    size_t GetVecSizeInBytes() const { return sizeof(DataType) * dim_; }

    void Save(LocalFileHandle &file_handle) const { file_handle.Append(&dim_, sizeof(dim_)); }

    static This Load(LocalFileHandle &file_handle) {
        size_t dim;
        file_handle.Read(&dim, sizeof(dim));
        return This(dim);
    }

    static This LoadFromPtr(const char *&ptr) {
        size_t dim = ReadBufAdv<size_t>(ptr);
        return This(dim);
    }

    QueryType MakeQuery(const DataType *vec) const { return vec; }

    size_t dim() const { return dim_; }

private:
    size_t dim_;

public:
    void Dump(std::ostream &os) const { os << "[CONST] dim: " << dim_ << std::endl; }
};

template <typename OtherDataType, bool OwnMem>
class PlainVecStoreInnerBase {
public:
    using This = PlainVecStoreInnerBase<OtherDataType, OwnMem>;
    using Meta = PlainVecStoreMeta<OtherDataType>;
    using Base = PlainVecStoreInnerBase<OtherDataType, OwnMem>;

public:
    PlainVecStoreInnerBase() = default;

    size_t GetSizeInBytes(size_t cur_vec_num, const Meta &meta) const { return sizeof(OtherDataType) * cur_vec_num * meta.dim(); }

    void Save(LocalFileHandle &file_handle, size_t cur_vec_num, const Meta &meta) const {
        file_handle.Append(ptr_.get(), sizeof(OtherDataType) * cur_vec_num * meta.dim());
    }

    static void
    SaveToPtr(LocalFileHandle &file_handle, const std::vector<const This *> &inners, const Meta &meta, size_t ck_size, size_t chunk_num, size_t last_chunk_size) {
        for (size_t i = 0; i < chunk_num; ++i) {
            size_t chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            file_handle.Append(inners[i]->ptr_.get(), sizeof(OtherDataType) * chunk_size * meta.dim());
        }
    }

    const OtherDataType *GetVec(size_t idx, const Meta &meta) const { return ptr_.get() + idx * meta.dim(); }

    void Prefetch(VertexType vec_i, const Meta &meta) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i, meta)), _MM_HINT_T0); }

protected:
    ArrayPtr<OtherDataType, OwnMem> ptr_;

public:
    void Dump(std::ostream &os, size_t offset, size_t chunk_size, const Meta &meta) const {
        for (int i = 0; i < (int)chunk_size; i++) {
            os << "vec " << i << "(" << offset + i << "): ";
            const OtherDataType *v = GetVec(i, meta);
            for (size_t j = 0; j < meta.dim(); j++) {
                os << v[j] << " ";
            }
            os << std::endl;
        }
    }
};

export template <typename DataType, bool OwnMem>
class PlainVecStoreInner : public PlainVecStoreInnerBase<DataType, OwnMem> {
public:
    using This = PlainVecStoreInner<DataType, OwnMem>;
    using Meta = PlainVecStoreMeta<DataType>;
    using Base = PlainVecStoreInnerBase<DataType, OwnMem>;

protected:
    PlainVecStoreInner(size_t max_vec_num, const Meta &meta) { this->ptr_ = std::make_unique<DataType[]>(max_vec_num * meta.dim()); }

public:
    PlainVecStoreInner() = default;

    static This Make(size_t max_vec_num, const Meta &meta, size_t &mem_usage) {
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return This(max_vec_num, meta);
    }

    static This Load(LocalFileHandle &file_handle, size_t cur_vec_num, size_t max_vec_num, const Meta &meta, size_t &mem_usage) {
        assert(cur_vec_num <= max_vec_num);
        This ret(max_vec_num, meta);
        file_handle.Read(ret.ptr_.get(), sizeof(DataType) * cur_vec_num * meta.dim());
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return ret;
    }

    static This LoadFromPtr(const char *&ptr, size_t cur_vec_num, size_t max_vec_num, const Meta &meta, size_t &mem_usage) {
        This ret(max_vec_num, meta);
        std::memcpy(ret.ptr_.get(), ptr, sizeof(DataType) * cur_vec_num * meta.dim());
        ptr += sizeof(DataType) * cur_vec_num * meta.dim();
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return ret;
    }

    void SetVec(size_t idx, const DataType *vec, const Meta &meta, size_t &mem_usage) { Copy(vec, vec + meta.dim(), GetVecMut(idx, meta)); }

private:
    DataType *GetVecMut(size_t idx, const Meta &meta) { return this->ptr_.get() + idx * meta.dim(); }
};

export template <typename DataType>
class PlainVecStoreInner<DataType, false> : public PlainVecStoreInnerBase<DataType, false> {
    using This = PlainVecStoreInner<DataType, false>;
    using Meta = PlainVecStoreMeta<DataType>;

protected:
    // PlainVecStoreInner(const DataType *ptr) { this->ptr_ = ptr; }

public:
    explicit PlainVecStoreInner(const DataType *ptr) { this->ptr_ = ptr; }
    PlainVecStoreInner() = default;
    static This LoadFromPtr(const char *&ptr, size_t cur_vec_num, const Meta &meta) {
        const auto *p = reinterpret_cast<const DataType *>(ptr); // fixme
        ptr += sizeof(DataType) * cur_vec_num * meta.dim();
        return This(p);
    }
};

} // namespace infinity