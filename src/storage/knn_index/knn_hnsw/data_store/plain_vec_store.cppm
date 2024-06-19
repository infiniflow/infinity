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
import file_system;
import hnsw_common;

namespace infinity {

export template <typename DataType>
class PlainVecStoreMeta {
public:
    using This = PlainVecStoreMeta<DataType>;
    using StoreType = const DataType *;
    using QueryType = const DataType *;

private:
    PlainVecStoreMeta(SizeT dim) : dim_(dim) {}

public:
    PlainVecStoreMeta() : dim_(0) {}
    PlainVecStoreMeta(This &&other) : dim_(std::exchange(other.dim_, 0)) {}

    static This Make(SizeT dim) { return This(dim); }
    static This Make(SizeT dim, bool) { return This(dim); }

    void Save(FileHandler &file_handler) const { file_handler.Write(&dim_, sizeof(dim_)); }

    static This Load(FileHandler &file_handler) {
        SizeT dim;
        file_handler.Read(&dim, sizeof(dim));
        return This(dim);
    }

    QueryType MakeQuery(const DataType *vec) const { return vec; }

    SizeT dim() const { return dim_; }

private:
    SizeT dim_;

public:
    void Dump(std::ostream &os) const { os << "[CONST] dim: " << dim_ << std::endl; }
};

export template <typename DataType>
class PlainVecStoreInner {
public:
    using This = PlainVecStoreInner<DataType>;
    using Meta = PlainVecStoreMeta<DataType>;

private:
    PlainVecStoreInner(SizeT max_vec_num, const Meta &meta) : ptr_(MakeUnique<DataType[]>(max_vec_num * meta.dim())) {}

public:
    PlainVecStoreInner() = default;

    static This Make(SizeT max_vec_num, const Meta &meta) { return This(max_vec_num, meta); }

    void Save(FileHandler &file_handler, SizeT cur_vec_num, const Meta &meta) const {
        file_handler.Write(ptr_.get(), sizeof(DataType) * cur_vec_num * meta.dim());
    }

    static This Load(FileHandler &file_handler, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta) {
        assert(cur_vec_num <= max_vec_num);
        This ret(max_vec_num, meta);
        file_handler.Read(ret.ptr_.get(), sizeof(DataType) * cur_vec_num * meta.dim());
        return ret;
    }

    void SetVec(SizeT idx, const DataType *vec, const Meta &meta) { Copy(vec, vec + meta.dim(), GetVecMut(idx, meta)); }

    const DataType *GetVec(SizeT idx, const Meta &meta) const { return ptr_.get() + idx * meta.dim(); }

    void Prefetch(VertexType vec_i, const Meta &meta) const { _mm_prefetch(reinterpret_cast<const char *>(GetVec(vec_i, meta)), _MM_HINT_T0); }

private:
    DataType *GetVecMut(SizeT idx, const Meta &meta) { return ptr_.get() + idx * meta.dim(); }

private:
    UniquePtr<DataType[]> ptr_;

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

} // namespace infinity