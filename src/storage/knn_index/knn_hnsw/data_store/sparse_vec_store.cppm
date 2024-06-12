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

#include <fstream>
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <xmmintrin.h>
#elif defined(__GNUC__) && defined(__aarch64__)
#include <simde/x86/sse.h>
#endif

export module sparse_vec_store;

import stl;
import file_system;
import hnsw_common;
import sparse_util;

namespace infinity {

export template <typename DataType, typename IdxType>
class SparseVecStoreMeta {
public:
    using This = SparseVecStoreMeta<DataType, IdxType>;
    using QueryVecType = SparseVecRef<DataType, IdxType>;
    using StoreType = SparseVecRef<DataType, IdxType>;
    using QueryType = SparseVecRef<DataType, IdxType>;

private:
    SparseVecStoreMeta(SizeT max_dim) : max_dim_(max_dim) {}

public:
    static This Make(SizeT max_dim) { return This(max_dim); }

    void Save(FileHandler &file_handler) const { file_handler.Write(&max_dim_, sizeof(max_dim_)); }

    static This Load(FileHandler &file_handler) {
        SizeT max_dim;
        file_handler.Read(&max_dim, sizeof(max_dim));
        return This(max_dim);
    }

    QueryType MakeQuery(QueryVecType vec) const { return vec; }

    SizeT dim() const { return max_dim_; }

private:
    SizeT max_dim_;

public:
    void Dump(std::ostream &os) const { os << "[CONST] max dim: " << max_dim_ << std::endl; }
};

template <typename DataType, typename IdxType>
struct SparseVecEle {
    i32 nnz_{};
    UniquePtr<IdxType[]> indices_;
    UniquePtr<DataType[]> data_;
};

export template <typename DataType, typename IdxType>
class SparseVecStoreInner {
public:
    using This = SparseVecStoreInner<DataType, IdxType>;
    using Meta = SparseVecStoreMeta<DataType, IdxType>;
    using SparseVecRef = SparseVecRef<DataType, IdxType>;
    using SparseVecEle = SparseVecEle<DataType, IdxType>;

private:
    SparseVecStoreInner(SizeT max_vec_num, const Meta &meta) : vecs_(MakeUniqueForOverwrite<SparseVecEle[]>(max_vec_num)) {}

public:
    SparseVecStoreInner() = default;

    static This Make(SizeT max_vec_num, const Meta &meta) { return This(max_vec_num, meta); }

    void Save(FileHandler &file_handler, SizeT cur_vec_num, const Meta &meta) const {
        SizeT nnz = 0;
        for (SizeT i = 0; i < cur_vec_num; ++i) {
            nnz += vecs_[i].nnz_;
        }
        file_handler.Write(&nnz, sizeof(nnz));
        auto indptr = MakeUniqueForOverwrite<i32[]>(cur_vec_num + 1);
        indptr[0] = 0;
        auto indice = MakeUniqueForOverwrite<IdxType[]>(nnz);
        auto data = MakeUniqueForOverwrite<DataType[]>(nnz);
        for (SizeT i = 0; i < cur_vec_num; ++i) {
            const SparseVecEle &vec = vecs_[i];
            Copy(vec.indices_.get(), vec.indices_.get() + vec.nnz_, indice.get() + indptr[i]);
            Copy(vec.data_.get(), vec.data_.get() + vec.nnz_, data.get() + indptr[i]);
            indptr[i + 1] = indptr[i] + vec.nnz_;
        }
        file_handler.Write(indptr.get(), sizeof(i32) * (cur_vec_num + 1));
        file_handler.Write(indice.get(), sizeof(IdxType) * nnz);
        file_handler.Write(data.get(), sizeof(DataType) * nnz);
    }

    static This Load(FileHandler &file_handler, SizeT cur_vec_num, SizeT max_vec_num, const Meta &meta) {
        SizeT nnz = 0;
        file_handler.Read(&nnz, sizeof(nnz));
        auto indptr = MakeUniqueForOverwrite<i32[]>(cur_vec_num + 1);
        file_handler.Read(indptr.get(), sizeof(i32) * (cur_vec_num + 1));
        auto indice = MakeUniqueForOverwrite<IdxType[]>(nnz);
        file_handler.Read(indice.get(), sizeof(IdxType) * nnz);
        auto data = MakeUniqueForOverwrite<DataType[]>(nnz);
        file_handler.Read(data.get(), sizeof(DataType) * nnz);

        This ret(max_vec_num, meta);
        for (SizeT i = 0; i < cur_vec_num; ++i) {
            SparseVecEle &vec = ret.vecs_[i];
            vec.nnz_ = indptr[i + 1] - indptr[i];
            vec.indices_ = MakeUniqueForOverwrite<IdxType[]>(vec.nnz_);
            vec.data_ = MakeUniqueForOverwrite<DataType[]>(vec.nnz_);
            Copy(indice.get() + indptr[i], indice.get() + indptr[i + 1], vec.indices_.get());
            Copy(data.get() + indptr[i], data.get() + indptr[i + 1], vec.data_.get());
        }
        return ret;
    }

    void SetVec(SizeT idx, const SparseVecRef &vec, const Meta &meta) {
        SparseVecEle &dst = vecs_[idx];
        dst.nnz_ = vec.nnz_;
        dst.indices_ = MakeUniqueForOverwrite<IdxType[]>(vec.nnz_);
        dst.data_ = MakeUniqueForOverwrite<DataType[]>(vec.nnz_);
        Copy(vec.indices_, vec.indices_ + vec.nnz_, dst.indices_.get());
        Copy(vec.data_, vec.data_ + vec.nnz_, dst.data_.get());
    }

    SparseVecRef GetVec(SizeT idx, const Meta &meta) const {
        const SparseVecEle &vec = vecs_[idx];
        return SparseVecRef(vec.nnz_, vec.indices_.get(), vec.data_.get());
    }

    void Prefetch(SizeT idx, const Meta &meta) const {
        const SparseVecEle &vec = vecs_[idx];
        _mm_prefetch(vec.indices_.get(), _MM_HINT_T0);
        _mm_prefetch(vec.data_.get(), _MM_HINT_T0);
    }

private:
    UniquePtr<SparseVecEle[]> vecs_;

public:
    void Dump(std::ostream &os, SizeT offset, SizeT chunk_size, const Meta &meta) const {
        for (int i = 0; i < (int)chunk_size; ++i) {
            os << "vec " << i << "(" << i + offset << "): ";
            const SparseVecEle &vec = vecs_[i];
            for (i32 j = 0; j < vec.nnz_; ++j) {
                os << vec.indices_[j] << ":" << vec.data_[j] << " ";
            }
            os << std::endl;
        }
    }
};

} // namespace infinity