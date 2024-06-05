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

#include <vector>

export module sparse_util;

import stl;
import sparse_vector_distance;
import knn_result_handler;

namespace infinity {

export struct SparseVecRef {
    const f32 *data_;
    const u32 *indices_;
    i32 nnz_;
};

export struct SparseMatrix {
public:
    SparseVecRef at(i64 row_id) const {
        i64 offset = indptr_[row_id];
        const float *data = data_.get() + offset;
        const auto *indices = reinterpret_cast<const u32 *>(indices_.get() + offset);
        i32 nnz = indptr_[row_id + 1] - offset;
        return SparseVecRef{data, indices, nnz};
    }

public:
    UniquePtr<f32[]> data_{};
    UniquePtr<i32[]> indices_{};
    UniquePtr<i64[]> indptr_{}; // row i's data and indice is stored in data_[indptr_[i]:indptr_[i+1]], indices_[indptr_[i]:indptr_[i+1]]
    i64 nrow_{};
    i64 ncol_{};
    i64 nnz_{};
};

export class SparseMatrixIter {
public:
    SparseMatrixIter(const SparseMatrix &mat) : mat_(mat) {}

    bool HasNext() { return row_i_ < mat_.nrow_; }

    void Next() {
        ++row_i_;
        offset_ = mat_.indptr_[row_i_];
    }

    SparseVecRef val() const {
        const float *data = mat_.data_.get() + offset_;
        const auto *indices = reinterpret_cast<const u32 *>(mat_.indices_.get() + offset_);
        i32 nnz = mat_.indptr_[row_i_ + 1] - offset_;
        return SparseVecRef{data, indices, nnz};
    }

    i64 row_id() const { return row_i_; }

private:
    const SparseMatrix &mat_;
    i64 row_i_{};
    i64 offset_{};
};

export struct SparseVecUtil {
    static f32 DistanceIP(const SparseVecRef &vec1, const SparseVecRef &vec2) {
        return SparseIPDistance(vec1.data_, vec1.indices_, vec1.nnz_, vec2.data_, vec2.indices_, vec2.nnz_);
    }

    static Pair<Vector<u32>, Vector<f32>> Rerank(const SparseMatrix &mat, const SparseVecRef &query, Vector<u32> candidates, u32 topk) {
        Vector<u32> result(topk);
        Vector<f32> result_score(topk);

        HeapResultHandler<CompareMin<f32, u32>> result_handler(1 /*query_n*/, topk, result_score.data(), result.data());
        for (u32 row_id : candidates) {
            SparseVecRef vec = mat.at(row_id);
            f32 score = SparseVecUtil::DistanceIP(query, vec);
            result_handler.AddResult(0 /*query_id*/, score, row_id);
        }
        result_handler.End(0 /*query_id*/);
        return {std::move(result), std::move(result_score)};
    }
};

} // namespace infinity