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
import bm_index;
import bm_posting;
import sparse_util;
import third_party;
import compilation_config;
import file_system;
import local_file_system;
import file_system_type;
import sparse_test_util;
import infinity_exception;

using namespace infinity;

class BMIndexTest : public BaseTest {
protected:
    void SetUp() override {}

    void TearDown() override {}

    template <typename BMIndex>
    void TestFunc(u32 block_size) {
        u32 nrow = 1000;
        u32 ncol = 1000;
        f32 sparsity = 0.05;
        u32 query_n = 100;
        u32 topk = 10;
        // u32 gt_size = std::min(nrow, topk);

        f32 alpha = 1.0;
        f32 beta = 1.0;

        f32 accuracy_all = 0.9;

        const SparseMatrix dataset = SparseTestUtil::GenerateDataset(nrow, ncol, sparsity, 0.0, 10.0);
        const SparseMatrix query_set = SparseTestUtil::GenerateDataset(query_n, ncol, sparsity, 0.0, 10.0);
        const auto [gt_indices_list, gt_scores_list] = SparseTestUtil::GenerateGroundtruth(dataset, query_set, topk, false);

        String save_path = String(tmp_data_path()) + "/bmindex_test1.index";
        LocalFileSystem fs;

        auto test_query = [&](const BMIndex &index) {
            u32 hit_all = 0;
            u32 total_all = 0;
            for (SparseMatrixIter iter(query_set); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();

                auto [indices, scores] = index.SearchKnn(vec, topk, alpha, beta);

                u32 query_id = iter.row_id();
                const i32 *gt_indices = gt_indices_list.get() + query_id * topk;
                const f32 *gt_scores = gt_scores_list.get() + query_id * topk;

                auto [hit, total] = SparseTestUtil::CheckApproximateKnn(gt_indices, gt_scores, topk, indices, scores);
                hit_all += hit;
                total_all += total;

                // SparseTestUtil::PrintQuery(query_id, gt_indices, gt_scores, gt_size, indices, scores);
                // std::cout << fmt::format("accuracy: {}\n", (f32)hit / total);
            }
            std::cout << fmt::format("hit: {}, total: {}\n", hit_all, total_all);
            if (hit_all < total_all * accuracy_all) {
                EXPECT_TRUE(false);
            }
        };
        {
            BMIndex index(ncol, block_size);
            for (SparseMatrixIter iter(dataset); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                index.AddDoc(vec);
            }

            test_query(index);
            index.Optimize(topk);
            test_query(index);

            auto [file_handler, status] = fs.OpenFile(save_path, FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            index.Save(*file_handler);
        }
        {
            auto [file_handler, status] = fs.OpenFile(save_path, FileFlags::READ_FLAG, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            auto index = BMIndex::Load(*file_handler);

            test_query(index);
        }
    }
};

TEST_F(BMIndexTest, test1) {
    {
        u32 block_size = 8;
        TestFunc<BMIndex<BMCompressType::kCompressed>>(block_size);
    }
    {
        u32 block_size = 64;
        TestFunc<BMIndex<BMCompressType::kRaw>>(block_size);
    }
}