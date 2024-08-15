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
#include <vector>

export module sparse_util;

import stl;
import sparse_vector_distance;
import knn_result_handler;
import file_system;
import infinity_exception;
import third_party;

namespace infinity {

export template <typename DataT, typename IdxT>
struct SparseVecRef {
    using DataType = DataT;
    using IdxType = IdxT;

    SparseVecRef(i32 nnz, const IdxT *indices, const DataT *data) : nnz_(nnz), indices_(indices), data_(data) {}

    i32 nnz_ = 0;
    const IdxType *indices_ = nullptr;
    const DataType *data_ = nullptr;
};

export template <typename DataT, typename IdxT>
struct SparseVec {
    i32 nnz_ = 0;
    UniquePtr<IdxT[]> indices_;
    UniquePtr<DataT[]> data_;

    SparseVecRef<DataT, IdxT> ToRef() const { return {nnz_, indices_.get(), data_.get()}; }
};

export template <typename DataType, typename IdxType>
struct SparseVecEle {
    SparseVecEle() = default;

    void Init(const Vector<SizeT> &keep_idxes, const DataType *data, const IdxType *indices) {
        nnz_ = keep_idxes.size();
        indices_ = MakeUniqueForOverwrite<IdxType[]>(nnz_);
        data_ = MakeUniqueForOverwrite<DataType[]>(nnz_);
        for (i32 i = 0; i < nnz_; ++i) {
            indices_[i] = indices[keep_idxes[i]];
            data_[i] = data[keep_idxes[i]];
        }
    }

    i32 nnz_{};
    UniquePtr<IdxType[]> indices_;
    UniquePtr<DataType[]> data_;
};

export template <typename DataT, typename IdxT>
struct SparseMatrix {
public:
    SparseVecRef<DataT, IdxT> at(i64 row_id) const {
        i64 offset = indptr_[row_id];
        i32 nnz = indptr_[row_id + 1] - offset;
        const auto *indices = reinterpret_cast<const IdxT *>(indices_.get() + offset);
        const float *data = data_.get() + offset;
        return SparseVecRef<DataT, IdxT>(nnz, indices, data);
    }

    static SparseMatrix Load(FileHandler &file_handler) {
        i64 nrow = 0;
        i64 ncol = 0;
        i64 nnz = 0;
        file_handler.Read(&nrow, sizeof(nrow));
        file_handler.Read(&ncol, sizeof(ncol));
        file_handler.Read(&nnz, sizeof(nnz));

        auto indptr = MakeUnique<i64[]>(nrow + 1);
        file_handler.Read(indptr.get(), sizeof(i64) * (nrow + 1));
        if (indptr[nrow] != nnz) {
            UnrecoverableError("Invalid indptr.");
        }

        auto indices = MakeUnique<IdxT[]>(nnz);
        file_handler.Read(indices.get(), sizeof(IdxT) * nnz);
        // assert all element in indices >= 0 and < ncol
        {
            bool check = std::all_of(indices.get(), indices.get() + nnz, [ncol](IdxT ele) { return ele >= 0 && ele < ncol; });
            if (!check) {
                UnrecoverableError("Invalid indices.");
            }
        }

        auto data = MakeUnique<DataT[]>(nnz);
        file_handler.Read(data.get(), sizeof(DataT) * nnz);
        return {std::move(data), std::move(indices), std::move(indptr), nrow, ncol, nnz};
    }

    void Save(FileHandler &file_handler) const {
        file_handler.Write(&nrow_, sizeof(nrow_));
        file_handler.Write(&ncol_, sizeof(ncol_));
        file_handler.Write(&nnz_, sizeof(nnz_));
        file_handler.Write(indptr_.get(), sizeof(i64) * (nrow_ + 1));
        file_handler.Write(indices_.get(), sizeof(IdxT) * nnz_);
        file_handler.Write(data_.get(), sizeof(DataT) * nnz_);
    }

    void Clear() {
        data_.reset();
        indices_.reset();
        indptr_.reset();
        nrow_ = 0;
        ncol_ = 0;
        nnz_ = 0;
    }

public:
    UniquePtr<DataT[]> data_{};
    UniquePtr<IdxT[]> indices_{};
    UniquePtr<i64[]> indptr_{}; // row i's data and indice is stored in data_[indptr_[i]:indptr_[i+1]], indices_[indptr_[i]:indptr_[i+1]]
    i64 nrow_{};
    i64 ncol_{};
    i64 nnz_{};
};

export template <typename DataT, typename IdxT>
class SparseMatrixIter {
public:
    SparseMatrixIter(const SparseMatrix<DataT, IdxT> &mat) : mat_(mat) {}

    bool HasNext() { return row_i_ < mat_.nrow_; }

    void Next() {
        ++row_i_;
        offset_ = mat_.indptr_[row_i_];
    }

    SparseVecRef<DataT, IdxT> val() const {
        i32 nnz = mat_.indptr_[row_i_ + 1] - offset_;
        const auto *indices = reinterpret_cast<const IdxT *>(mat_.indices_.get() + offset_);
        const auto *data = mat_.data_.get() + offset_;
        return SparseVecRef<DataT, IdxT>(nnz, indices, data);
    }

    i64 row_id() const { return row_i_; }

private:
    const SparseMatrix<DataT, IdxT> &mat_;
    i64 row_i_{};
    i64 offset_{};
};

export struct SparseVecUtil {
    template <typename DataT, typename IdxT>
    static DataT DistanceIP(const SparseVecRef<DataT, IdxT> &vec1, const SparseVecRef<DataT, IdxT> &vec2) {
        return SparseIPDistance(vec1.data_, vec1.indices_, vec1.nnz_, vec2.data_, vec2.indices_, vec2.nnz_);
    }

    template <typename DataT, typename IdxT>
    static Pair<Vector<u32>, Vector<DataT>>
    Rerank(const SparseMatrix<DataT, IdxT> &mat, const SparseVecRef<DataT, IdxT> &query, Vector<u32> candidates, u32 topk) {
        Vector<u32> result(topk);
        Vector<DataT> result_score(topk);

        HeapResultHandler<CompareMin<DataT, u32>> result_handler(1 /*query_n*/, topk, result_score.data(), result.data());
        for (u32 row_id : candidates) {
            SparseVecRef<DataT, IdxT> vec = mat.at(row_id);
            DataT score = SparseVecUtil::DistanceIP(query, vec);
            result_handler.AddResult(0 /*query_id*/, score, row_id);
        }
        result_handler.End(0 /*query_id*/);
        return {std::move(result), std::move(result_score)};
    }
};

} // namespace infinity