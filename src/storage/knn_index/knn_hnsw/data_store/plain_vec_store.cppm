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

export module plain_vec_store;

import stl;
import local_file_handle;
import hnsw_common;
import serialize;
import data_store_util;

namespace infinity {

export template <typename DataType>
class PlainVecStoreMeta {
public:
    using This = PlainVecStoreMeta<DataType>;
    using StoreType = const DataType *;
    using QueryType = const DataType *;
    using DistanceType = f32;

private:
    PlainVecStoreMeta(SizeT dim) : dim_(dim) {}

public:
    PlainVecStoreMeta() : dim_(0) {}
    PlainVecStoreMeta(This &&other) : dim_(std::exchange(other.dim_, 0)) {}
    PlainVecStoreMeta &operator=(This &&other) {
        if (this != &other) {
            dim_ = std::exchange(other.dim_, 0);
        }
        return *this;
    }

    static This Make(SizeT dim) { return This(dim); }
    static This Make(SizeT dim, bool) { return This(dim); }

    SizeT GetSizeInBytes() const { return sizeof(SizeT); }

    void Save(LocalFileHandle &file_handle) const { file_handle.Append(&dim_, sizeof(dim_)); }

    static This Load(LocalFileHandle &file_handle) {
        SizeT dim;
        file_handle.Read(&dim, sizeof(dim));
        return This(dim);
    }

    static This LoadFromPtr(const char *&ptr) {
        SizeT dim = ReadBufAdv<SizeT>(ptr);
        return This(dim);
    }

    QueryType MakeQuery(const DataType *vec) const { return vec; }

    SizeT dim() const { return dim_; }

private:
    SizeT dim_;

public:
    void Dump(std::ostream &os) const { os << "[CONST] dim: " << dim_ << std::endl; }
};

template <typename DataType, bool OwnMem>
class PlainVecStoreInnerBase {
public:
    using This = PlainVecStoreInnerBase<DataType, OwnMem>;
    using Meta = PlainVecStoreMeta<DataType>;
    using Base = PlainVecStoreInnerBase<DataType, OwnMem>;

public:
    PlainVecStoreInnerBase() = default;

    SizeT GetSizeInBytes(SizeT cur_vec_num, const Meta &meta) const { return sizeof(DataType) * cur_vec_num * meta.dim(); }

    void Save(LocalFileHandle &file_handle, SizeT cur_vec_num, const Meta &meta) const {
        file_handle.Append(ptr_.get(), sizeof(DataType) * cur_vec_num * meta.dim());
    }

    static void
    SaveToPtr(LocalFileHandle &file_handle, const Vector<const This *> &inners, const Meta &meta, SizeT ck_size, SizeT chunk_num, SizeT last_chunk_size) {
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            file_handle.Append(inners[i]->ptr_.get(), sizeof(DataType) * chunk_size * meta.dim());
        }
    }

    const DataType *GetVec(SizeT idx, const Meta &meta) const { return ptr_.get() + idx * meta.dim(); }

    void Prefetch(VertexType vec_i, const Meta &meta) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i, meta)), _MM_HINT_T0); }

protected:
    ArrayPtr<DataType, OwnMem> ptr_;

public:
    void Dump(std::ostream &os, SizeT offset, SizeT chunk_size, const Meta &meta) const {
        for (int i = 0; i < (int)chunk_size; i++) {
            os << "vec " << i << "(" << offset + i << "): ";
            const DataType *v = GetVec(i, meta);
            for (SizeT j = 0; j < meta.dim(); j++) {
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
    PlainVecStoreInner(SizeT max_vec_num, const Meta &meta) { this->ptr_ = MakeUnique<DataType[]>(max_vec_num * meta.dim()); }

public:
    PlainVecStoreInner() = default;

    static This Make(SizeT max_vec_num, const Meta &meta, SizeT &mem_usage) {
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return This(max_vec_num, meta);
    }

    static This Load(LocalFileHandle &file_handle, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta, SizeT &mem_usage) {
        assert(cur_vec_num <= max_vec_num);
        This ret(max_vec_num, meta);
        file_handle.Read(ret.ptr_.get(), sizeof(DataType) * cur_vec_num * meta.dim());
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return ret;
    }

    static This LoadFromPtr(const char *&ptr, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta, SizeT &mem_usage) {
        This ret(max_vec_num, meta);
        std::memcpy(ret.ptr_.get(), ptr, sizeof(DataType) * cur_vec_num * meta.dim());
        ptr += sizeof(DataType) * cur_vec_num * meta.dim();
        mem_usage += sizeof(DataType) * max_vec_num * meta.dim();
        return ret;
    }

    void SetVec(SizeT idx, const DataType *vec, const Meta &meta, SizeT &mem_usage) { Copy(vec, vec + meta.dim(), GetVecMut(idx, meta)); }

private:
    DataType *GetVecMut(SizeT idx, const Meta &meta) { return this->ptr_.get() + idx * meta.dim(); }
};

export template <typename DataType>
class PlainVecStoreInner<DataType, false> : public PlainVecStoreInnerBase<DataType, false> {
    using This = PlainVecStoreInner<DataType, false>;
    using Meta = PlainVecStoreMeta<DataType>;

protected:
    PlainVecStoreInner(const DataType *ptr) { this->ptr_ = ptr; }

public:
    PlainVecStoreInner() = default;
    static This LoadFromPtr(const char *&ptr, SizeT cur_vec_num, const Meta &meta) {
        const auto *p = reinterpret_cast<const DataType *>(ptr); // fixme
        ptr += sizeof(DataType) * cur_vec_num * meta.dim();
        return This(p);
    }
};

} // namespace infinity