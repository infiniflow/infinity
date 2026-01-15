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

#include <common/simd/simd_functions.h>

export module infinity_core:plain_vec_store;

import :local_file_handle;
import :hnsw_common;
import :data_store_util;
import :boost;

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
    using segment_manager = boost::interprocess::managed_mapped_file::segment_manager;

private:
    PlainVecStoreMeta(size_t dim) : dim_(dim) {}

public:
    PlainVecStoreMeta(segment_manager *sm) : dim_(0) {}

    static This Make(size_t dim, segment_manager *sm) { return This(dim); }
    static This Make(size_t dim, bool normalize, segment_manager *sm) { return This(dim); }

    size_t GetSizeInBytes() const { return sizeof(size_t); }

    // Get size of vector in search
    size_t GetVecSizeInBytes() const { return sizeof(DataType) * dim_; }

    size_t CalcSize() const {
        size_t ret{};

        ret += sizeof(dim_);

        return ret;
    }

    void SaveToPtr(void *&mmap_p, size_t &offset) const {
        std::memcpy((char *)mmap_p + offset, &dim_, sizeof(dim_));
        offset += sizeof(dim_);
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

template <typename OtherDataType>
class PlainVecStoreInnerBase {
public:
    using This = PlainVecStoreInnerBase<OtherDataType>;
    using Meta = PlainVecStoreMeta<OtherDataType>;

    using segment_manager = boost::interprocess::managed_mapped_file::segment_manager;

    // PlainVecStoreInnerBase() = default;
    PlainVecStoreInnerBase(segment_manager *sm) : ptr_(sm), sm_(sm) {}

    size_t GetSizeInBytes(size_t cur_vec_num, const Meta &meta) const { return sizeof(OtherDataType) * cur_vec_num * meta.dim(); }

    static size_t CalcSize(const Meta &meta, size_t ck_size, size_t chunk_num, size_t last_chunk_size) {
        size_t ret{};
        for (size_t i = 0; i < chunk_num; ++i) {
            size_t chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            ret += sizeof(OtherDataType) * chunk_size * meta.dim();
        }
        return ret;
    }

    static void SaveToPtr(void *&mmap_p,
                          size_t &offset,
                          const std::vector<const This *> &inners,
                          const Meta &meta,
                          size_t ck_size,
                          size_t chunk_num,
                          size_t last_chunk_size) {
        for (size_t i = 0; i < chunk_num; ++i) {
            size_t chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            std::memcpy((char *)mmap_p + offset, inners[i]->ptr_.get(), sizeof(OtherDataType) * chunk_size * meta.dim());
            offset += sizeof(OtherDataType) * chunk_size * meta.dim();
        }
    }

    const OtherDataType *GetVec(size_t idx, const Meta &meta) const { return ptr_.data() + idx * meta.dim(); }

    const OtherDataType *GetVecToQuery(size_t idx, const Meta &meta) const { return GetVec(idx, meta); }

    void Prefetch(VertexType vec_i, const Meta &meta) const { SIMDPrefetch(reinterpret_cast<const void *>(GetVec(vec_i, meta))); }

protected:
    // ArrayPtr<OtherDataType> ptr_;
    boost::interprocess::vector<OtherDataType, boost::interprocess::allocator<OtherDataType, segment_manager>> ptr_;
    segment_manager *sm_;

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

export template <typename DataType>
class PlainVecStoreInner : public PlainVecStoreInnerBase<DataType> {
public:
    using This = PlainVecStoreInner<DataType>;
    using Meta = PlainVecStoreMeta<DataType>;
    using Base = PlainVecStoreInnerBase<DataType>;

    using segment_manager = boost::interprocess::managed_mapped_file::segment_manager;

protected:
    PlainVecStoreInner(size_t max_vec_num, const Meta &meta, segment_manager *sm) : Base(sm) {
        // this->ptr_ = std::make_unique<DataType[]>(max_vec_num * meta.dim());
        this->ptr_.resize(max_vec_num * meta.dim());
    }

public:
    // PlainVecStoreInner() = default;
    PlainVecStoreInner(segment_manager *sm) : Base(sm) {}

    static This Make(size_t max_vec_num, const Meta &meta, size_t &mem_usage, segment_manager *sm) {
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return This(max_vec_num, meta, sm);
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

    void SetVec(size_t idx, const DataType *vec, const Meta &meta, size_t &mem_usage) { std::copy(vec, vec + meta.dim(), GetVecMut(idx, meta)); }

private:
    DataType *GetVecMut(size_t idx, const Meta &meta) { return this->ptr_.data() + idx * meta.dim(); }
};

// export template <typename DataType>
// class PlainVecStoreInner<DataType, false> : public PlainVecStoreInnerBase<DataType, false> {
//     using This = PlainVecStoreInner<DataType, false>;
//     using Meta = PlainVecStoreMeta<DataType>;
//
// protected:
//     // PlainVecStoreInner(const DataType *ptr) { this->ptr_ = ptr; }
//
// public:
//     explicit PlainVecStoreInner(const DataType *ptr) { this->ptr_ = ptr; }
//     PlainVecStoreInner() = default;
//     static This LoadFromPtr(const char *&ptr, size_t cur_vec_num, const Meta &meta) {
//         const auto *p = reinterpret_cast<const DataType *>(ptr); // fixme
//         ptr += sizeof(DataType) * cur_vec_num * meta.dim();
//         return This(p);
//     }
// };

} // namespace infinity