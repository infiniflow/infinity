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
import sparse_test_util;

using namespace infinity;

class LinScanAlgTest : public BaseTest {
protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(LinScanAlgTest, accurate_scan) {
    u32 nrow = 1000;
    u32 ncol = 10000;
    f32 sparsity = 0.01;
    u32 query_n = 50;
    u32 topk = 10;
    f32 error_bound = 1e-6;

    u32 gt_size = std::min(nrow, topk);

    const SparseMatrix dataset = SparseTestUtil::GenerateDataset(nrow, ncol, sparsity);
    const SparseMatrix query_set = SparseTestUtil::GenerateDataset(query_n, ncol, sparsity);
    const auto [gt_indices_list, gt_scores_list] = SparseTestUtil::GenerateGroundtruth(dataset, query_set, topk, false);

    LinScan<f32, i32> index(dataset.ncol_);
    for (auto iter = SparseMatrixIter(dataset); iter.HasNext(); iter.Next()) {
        SparseVecRef vec = iter.val();
        u32 row_id = iter.row_id();
        index.Insert(vec, row_id);
    }

    for (auto iter = SparseMatrixIter(query_set); iter.HasNext(); iter.Next()) {
        SparseVecRef query = iter.val();

        auto [indices, scores] = index.SearchBF(query, topk);

        u32 query_id = iter.row_id();
        const i32 *gt_indices = gt_indices_list.get() + query_id * topk;
        const f32 *gt_scores = gt_scores_list.get() + query_id * topk;

        bool ck = SparseTestUtil::CheckAccurateKnn(gt_indices, gt_scores, gt_size, indices, scores, error_bound);
        if (!ck) {
            SparseTestUtil::PrintQuery(query_id, gt_indices, gt_scores, gt_size, indices, scores);
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

    const SparseMatrix dataset = SparseTestUtil::GenerateDataset(nrow, ncol, sparsity);
    const SparseMatrix query = SparseTestUtil::GenerateDataset(query_n, ncol, sparsity);
    const auto [gt_indices_list, gt_scores_list] = SparseTestUtil::GenerateGroundtruth(dataset, query, topk, false);

    LinScan<f32, i32> index(dataset.ncol_);
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

        auto [candidate_indices, candidate_scores, used_budget] = index.SearchKnn(query, candidate_n, budget);
        used_budget_all += used_budget;

        // std::cout << fmt::format("budget: {}, used: {}\n", budget, used_budget);

        auto [indices, scores] = SparseVecUtil::Rerank(dataset, query, candidate_indices, topk);

        u32 query_id = iter.row_id();
        const i32 *gt_indices = gt_indices_list.get() + query_id * topk;
        const f32 *gt_scores = gt_scores_list.get() + query_id * topk;

        auto [hit, total] = SparseTestUtil::CheckApproximateKnn(gt_indices, gt_scores, gt_size, indices, scores);
        hit_all += hit;
        total_all += total;

        // SparseTestUtil::PrintQuery(query_id, gt_indices, gt_scores, gt_size, indices, scores);
        std::cout << fmt::format("accuracy: {}\n", (f32)hit / total);
    }
    if (hit_all < total_all * accuracy_all) {
        std::cout << fmt::format("hit: {}, total: {}\n", hit_all, total_all);
        EXPECT_TRUE(false);
    }
    std::cout << fmt::format("All accuracy: {}\n", (f32)hit_all / total_all);
    std::cout << fmt::format("avg budget: {}\n", (f32)used_budget_all / query_n);
}
