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

export module sparse_test_util;

import stl;
import sparse_vector_distance;
import knn_result_handler;
import file_system;
import linscan_alg;
import sparse_util;
import infinity_exception;

namespace infinity {

export struct SparseTestUtil {
    static SparseMatrix<f32, i32> GenerateDataset(u32 nrow, u32 ncol, f32 sparsity = 0.01, f32 data_min = -10.0, f32 data_max = 10.0) {
        std::mt19937 rng{std::random_device{}()};

        if (sparsity < 0.0 || sparsity > 1.0) {
            UnrecoverableError("Invalid sparsity.");
        }
        if (data_min >= data_max) {
            UnrecoverableError("Invalid data range.");
        }
        if (nrow == 0 || ncol == 0) {
            UnrecoverableError("Invalid dimension.");
        }
        u32 nnz = nrow * ncol * sparsity;
        auto data = MakeUnique<f32[]>(nnz);
        auto indices = MakeUnique<i32[]>(nnz);
        auto indptr = MakeUnique<i64[]>(nrow + 1);
        {
            std::uniform_real_distribution<f32> distrib(data_min, data_max);
            for (u32 i = 0; i < nnz; ++i) {
                data[i] = distrib(rng);
            }
        }
        while (true) {
            indptr[0] = 0;
            indptr[nrow] = nnz;
            // get nrow - 1 random number between 0 and nnz
            for (u32 i = 1; i < nrow; ++i) {
                indptr[i] = rng() % (nnz + 1);
            }
            std::sort(indptr.get(), indptr.get() + nrow);
            bool check = true;
            for (u32 i = 0; i < nrow; ++i) {
                if (indptr[i + 1] - indptr[i] > ncol) {
                    check = false;
                    break;
                }
            }
            if (check) {
                break;
            }
        }
        {
            i64 start = indptr[0];
            for (u32 i = 0; i < nrow; ++i) {
                i64 end = indptr[i + 1];
                assert(end - start <= ncol);
                HashSet<i32> indice_set;
                for (i64 j = start; j < end;) {
                    i32 r = rng() % ncol;
                    if (auto [iter, insert_ok] = indice_set.emplace(r); insert_ok) {
                        ++j;
                    }
                }
                assert((i64)indice_set.size() == end - start);
                i64 j = start;
                for (i32 indice : indice_set) {
                    indices[j++] = indice;
                }
                std::sort(indices.get() + start, indices.get() + end);
                start = end;
            }
        }
        return SparseMatrix<f32, i32>{std::move(data), std::move(indices), std::move(indptr), nrow, ncol, nnz};
    }

    static Pair<UniquePtr<i32[]>, UniquePtr<f32[]>>
    GenerateGroundtruth(const SparseMatrix<f32, i32> &mat, const SparseMatrix<f32, i32> &query, u32 topk, bool use_linscan = true) {
        if (mat.ncol_ != query.ncol_) {
            UnrecoverableError("Inconsistent dimension.");
        }
        auto gt_indices = MakeUnique<i32[]>(query.nrow_ * topk);
        auto gt_scores = MakeUnique<f32[]>(query.nrow_ * topk);
        if (use_linscan) {
            LinScan<f32, i32> index;
            for (auto iter = SparseMatrixIter<f32, i32>(mat); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                u32 row_id = iter.row_id();
                index.Insert(vec, row_id);
            }
            for (auto iter = SparseMatrixIter<f32, i32>(query); iter.HasNext(); iter.Next()) {
                SparseVecRef query = iter.val();
                auto [indices, scores] = index.SearchBF(query, topk);
                u32 query_id = iter.row_id();
                Copy(indices.begin(), indices.end(), gt_indices.get() + query_id * topk);
                Copy(scores.begin(), scores.end(), gt_scores.get() + query_id * topk);
            }
        } else { // brute force, only used in linscan test
            for (auto iter = SparseMatrixIter<f32, i32>(query); iter.HasNext(); iter.Next()) {
                SparseVecRef query = iter.val();
                auto [indices, scores] = SparseTestUtil::BruteForceKnn(mat, query, topk);
                u32 query_id = iter.row_id();
                Copy(indices.begin(), indices.end(), gt_indices.get() + query_id * topk);
                Copy(scores.begin(), scores.end(), gt_scores.get() + query_id * topk);
            }
        }
        return {std::move(gt_indices), std::move(gt_scores)};
    }

private:
    static Pair<Vector<u32>, Vector<f32>> BruteForceKnn(const SparseMatrix<f32, i32> &mat, const SparseVecRef<f32, i32> &query, u32 topk) {
        Vector<f32> scores(mat.nrow_, 0.0);
        for (auto iter = SparseMatrixIter<f32, i32>(mat); iter.HasNext(); iter.Next()) {
            SparseVecRef vec = iter.val();
            u32 row_id = iter.row_id();
            f32 score = SparseVecUtil::DistanceIP(query, vec);
            scores[row_id] = score;
        }
        Vector<u32> indices(mat.nrow_);
        std::iota(indices.begin(), indices.end(), 0);
        u32 result_size = std::min(topk, (u32)mat.nrow_);
        std::partial_sort(indices.begin(), indices.begin() + result_size, indices.end(), [&](u32 i, u32 j) { return scores[i] > scores[j]; });
        indices.resize(result_size);
        Vector<f32> top_scores(result_size);
        std::transform(indices.begin(), indices.end(), top_scores.begin(), [&](u32 i) { return scores[i]; });
        return {std::move(indices), std::move(top_scores)};
    }
};
} // namespace infinity