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

#include <iostream>
#include <vector>

module sparse_util;

import stl;
import file_system;
import infinity_exception;
import third_party;

namespace infinity {

SparseMatrix SparseMatrix::Load(FileHandler &file_handler) {
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

    auto indices = MakeUnique<i32[]>(nnz);
    file_handler.Read(indices.get(), sizeof(i32) * nnz);
    // assert all element in indices >= 0 and < ncol
    {
        bool check = std::all_of(indices.get(), indices.get() + nnz, [ncol](i32 ele) { return ele >= 0 && ele < ncol; });
        if (!check) {
            UnrecoverableError("Invalid indices.");
        }
    }

    auto data = MakeUnique<f32[]>(nnz);
    file_handler.Read(data.get(), sizeof(f32) * nnz);
    return {std::move(data), std::move(indices), std::move(indptr), nrow, ncol, nnz};
}

void SparseMatrix::Save(FileHandler &file_handler) const {
    file_handler.Write(&nrow_, sizeof(nrow_));
    file_handler.Write(&ncol_, sizeof(ncol_));
    file_handler.Write(&nnz_, sizeof(nnz_));
    file_handler.Write(indptr_.get(), sizeof(i64) * (nrow_ + 1));
    file_handler.Write(indices_.get(), sizeof(i32) * nnz_);
    file_handler.Write(data_.get(), sizeof(f32) * nnz_);
}

Pair<Vector<i32>, Vector<f32>> SparseVecUtil::Rerank(const SparseMatrix &mat, const SparseVecRef<f32, i32> &query, Vector<i32> candidates, u32 topk) {
    Vector<i32> result(topk);
    Vector<f32> result_score(topk);

    HeapResultHandler<CompareMin<f32, i32>> result_handler(1 /*query_n*/, topk, result_score.data(), result.data());
    for (u32 row_id : candidates) {
        SparseVecRef<f32, i32> vec = mat.at(row_id);
        f32 score = SparseVecUtil::DistanceIP(query, vec);
        result_handler.AddResult(0 /*query_id*/, score, row_id);
    }
    result_handler.End(0 /*query_id*/);
    return {std::move(result), std::move(result_score)};
}

} // namespace infinity
