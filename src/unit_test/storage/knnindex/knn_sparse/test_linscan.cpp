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

#include "unit_test/base_test.h"
#include <cassert>

import stl;
import linscan_alg;
import sparse_util;
import third_party;

using namespace infinity;

class LinScanAlgTest : public BaseTest {
protected:
    void SetUp() override {}

    void TearDown() override {}

    SparseMatrix GenerateDataset(u32 nrow, u32 ncol, f32 sparsity = 0.01, f32 data_min = -10.0, f32 data_max = 10.0) {
        if (sparsity < 0.0 || sparsity > 1.0) {
            throw std::runtime_error("Invalid sparsity.");
        }
        if (data_min >= data_max) {
            throw std::runtime_error("Invalid data range.");
        }
        if (nrow == 0 || ncol == 0) {
            throw std::runtime_error("Invalid dimension.");
        }
        u32 nnz = nrow * ncol * sparsity;
        auto data = MakeUnique<f32[]>(nnz);
        auto indices = MakeUnique<i32[]>(nnz);
        auto indptr = MakeUnique<i64[]>(nrow + 1);
        {
            std::uniform_real_distribution<f32> distrib(data_min, data_max);
            for (u32 i = 0; i < nnz; ++i) {
                data[i] = distrib(rng_);
            }
        }
        while (true) {
            indptr[0] = 0;
            indptr[nrow] = nnz;
            // get nrow - 1 random number between 0 and nnz
            for (u32 i = 1; i < nrow; ++i) {
                indptr[i] = rng_() % (nnz + 1);
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
                    i32 r = rng_() % ncol;
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
        return SparseMatrix{std::move(data), std::move(indices), std::move(indptr), nrow, ncol, nnz};
    }

    Pair<UniquePtr<u32[]>, UniquePtr<f32[]>>
    GenerateGroundtruth(const SparseMatrix &mat, const SparseMatrix &query, u32 topk, bool use_linscan = true) {
        if (mat.ncol_ != query.ncol_) {
            throw std::runtime_error("Inconsistent dimension.");
        }
        auto gt_indices = MakeUnique<u32[]>(query.nrow_ * topk);
        auto gt_scores = MakeUnique<f32[]>(query.nrow_ * topk);
        if (use_linscan) {
            LinScan index;
            for (auto iter = SparseMatrixIter(mat); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                u32 row_id = iter.row_id();
                index.Insert(vec, row_id);
            }
            for (auto iter = SparseMatrixIter(query); iter.HasNext(); iter.Next()) {
                SparseVecRef query = iter.val();
                auto [indices, scores] = index.SearchBF(query, topk);
                u32 query_id = iter.row_id();
                std::copy(indices.begin(), indices.end(), gt_indices.get() + query_id * topk);
                std::copy(scores.begin(), scores.end(), gt_scores.get() + query_id * topk);
            }
        } else { // brute force, only used in linscan test
            for (auto iter = SparseMatrixIter(query); iter.HasNext(); iter.Next()) {
                SparseVecRef query = iter.val();
                auto [indices, scores] = BruteForceKnn(mat, query, topk);
                u32 query_id = iter.row_id();
                std::copy(indices.begin(), indices.end(), gt_indices.get() + query_id * topk);
                std::copy(scores.begin(), scores.end(), gt_scores.get() + query_id * topk);
            }
        }
        return {std::move(gt_indices), std::move(gt_scores)};
    }

    bool CheckAccurateKnn(const u32 *gt_indices,
                          const f32 *gt_scores,
                          u32 gt_size,
                          const Vector<u32> &indices,
                          const Vector<f32> &scores,
                          f32 error_bound) {
        if (gt_size < indices.size()) {
            return false;
        }
        for (u32 i = 0; i < indices.size(); ++i) {
            if (gt_scores[i] == 0.0) {
                break;
            }
            if (std::abs(gt_scores[i] - scores[i]) > error_bound || gt_indices[i] != indices[i]) {
                return false;
            }
        }
        return true;
    }

    Pair<u32, u32>
    CheckApproximateKnn(const u32 *gt_indices, const f32 *gt_scores, u32 gt_size, const Vector<u32> &indices, const Vector<f32> &scores) {
        HashSet<u32> gt_set;
        for (u32 i = 0; i < gt_size; ++i) {
            if (gt_scores[i] == 0.0) {
                break;
            }
            gt_set.emplace(gt_indices[i]);
        }
        u32 hit = 0;
        for (u32 i = 0; i < indices.size(); ++i) {
            if (gt_set.find(indices[i]) != gt_set.end()) {
                ++hit;
            }
        }
        return {hit, gt_set.size()};
    }

    void PrintQuery(u32 query_id, const u32 *gt_indices, const f32 *gt_scores, u32 gt_size, const Vector<u32> &indices, const Vector<f32> &scores) {
        std::cout << fmt::format("Query {}\n", query_id);
        std::cout << "Result:\n";
        for (u32 i = 0; i < indices.size(); ++i) {
            std::cout << fmt::format("{} {}, ", indices[i], scores[i]);
        }
        std::cout << "\n";
        std::cout << "Groundtruth:\n";
        for (u32 i = 0; i < gt_size; ++i) {
            std::cout << fmt::format("{} {}, ", gt_indices[i], gt_scores[i]);
        }
        std::cout << "\n";
    }

private:
    Pair<Vector<u32>, Vector<f32>> BruteForceKnn(const SparseMatrix &mat, const SparseVecRef &query, u32 topk) {
        Vector<f32> scores(mat.nrow_, 0.0);
        for (auto iter = SparseMatrixIter(mat); iter.HasNext(); iter.Next()) {
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

private:
    std::mt19937 rng_{std::random_device{}()};
};

TEST_F(LinScanAlgTest, accurate_scan) {
    u32 nrow = 1000;
    u32 ncol = 10000;
    f32 sparsity = 0.01;
    u32 query_n = 50;
    u32 topk = 10;
    f32 error_bound = 1e-6;

    u32 gt_size = std::min(nrow, topk);

    const SparseMatrix dataset = GenerateDataset(nrow, ncol, sparsity);
    const SparseMatrix query_set = GenerateDataset(query_n, ncol, sparsity);
    const auto [gt_indices_list, gt_scores_list] = GenerateGroundtruth(dataset, query_set, topk, false);

    LinScan index;
    for (auto iter = SparseMatrixIter(dataset); iter.HasNext(); iter.Next()) {
        SparseVecRef vec = iter.val();
        u32 row_id = iter.row_id();
        index.Insert(vec, row_id);
    }

    for (auto iter = SparseMatrixIter(query_set); iter.HasNext(); iter.Next()) {
        SparseVecRef query = iter.val();

        auto [indices, scores] = index.SearchBF(query, topk);

        u32 query_id = iter.row_id();
        const u32 *gt_indices = gt_indices_list.get() + query_id * topk;
        const f32 *gt_scores = gt_scores_list.get() + query_id * topk;

        bool ck = CheckAccurateKnn(gt_indices, gt_scores, gt_size, indices, scores, error_bound);
        if (!ck) {
            PrintQuery(query_id, gt_indices, gt_scores, gt_size, indices, scores);
            EXPECT_TRUE(false);
        }
    }
}

TEST_F(LinScanAlgTest, approximate_scan) {
    u32 nrow = 1000;
    u32 ncol = 10000;
    f32 sparsity = 0.01;
    u32 query_n = 50;
    u32 topk = 10;

    f32 accuracy_all = 0.9;

    i32 budget = nrow * ncol * sparsity * sparsity;
    u32 candidate_n = topk * 3;

    u32 gt_size = std::min(nrow, topk);

    const SparseMatrix dataset = GenerateDataset(nrow, ncol, sparsity);
    const SparseMatrix query = GenerateDataset(query_n, ncol, sparsity);
    const auto [gt_indices_list, gt_scores_list] = GenerateGroundtruth(dataset, query, topk, false);

    LinScan index;
    for (auto iter = SparseMatrixIter(dataset); iter.HasNext(); iter.Next()) {
        SparseVecRef vec = iter.val();
        u32 row_id = iter.row_id();
        index.Insert(vec, row_id);
    }

    u32 hit_all = 0;
    u32 total_all = 0;
    u64 used_budget_all = 0;
    for (auto iter = SparseMatrixIter(query); iter.HasNext(); iter.Next()) {
        SparseVecRef query = iter.val();

        auto [candidate_indices, used_budget] = index.SearchKnn(query, candidate_n, budget);
        used_budget_all += used_budget;

        // std::cout << fmt::format("budget: {}, used: {}\n", budget, used_budget);

        auto [indices, scores] = SparseVecUtil::Rerank(dataset, query, candidate_indices, topk);

        u32 query_id = iter.row_id();
        const u32 *gt_indices = gt_indices_list.get() + query_id * topk;
        const f32 *gt_scores = gt_scores_list.get() + query_id * topk;

        auto [hit, total] = CheckApproximateKnn(gt_indices, gt_scores, gt_size, indices, scores);
        hit_all += hit;
        total_all += total;

        // PrintQuery(query_id, gt_indices, gt_scores, gt_size, indices, scores);
        std::cout << fmt::format("accuracy: {}\n", (f32)hit / total);
    }
    if (hit_all < total_all * accuracy_all) {
        std::cout << fmt::format("hit: {}, total: {}\n", hit_all, total_all);
        EXPECT_TRUE(false);
    }
    std::cout << fmt::format("All accuracy: {}\n", (f32)hit_all / total_all);
    std::cout << fmt::format("avg budget: {}\n", (f32)used_budget_all / query_n);
}
