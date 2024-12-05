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

#include <cassert>

#include "gtest/gtest.h"

import base_test;
import stl;
import bmp_alg;
import bmp_util;
import sparse_util;
import third_party;
import compilation_config;
import sparse_test_util;
import infinity_exception;
import virtual_store;
import local_file_handle;

using namespace infinity;

class BMPIndexTest : public BaseTest {
protected:
    template <typename DataType, typename IdxType, BMPCompressType CompressType>
    void TestFunc(u32 block_size) {
        using BMPAlg = BMPAlg<DataType, IdxType, CompressType>;

        u32 nrow = 1000;
        u32 ncol = 1000;
        f32 sparsity = 0.05;
        u32 query_n = 100;
        u32 topk = 10;
        // u32 gt_size = std::min(nrow, topk);

        BmpSearchOptions options;
        options.use_lock_ = false;

        f32 accuracy_all = 0.9;

        const SparseMatrix dataset = SparseTestUtil<DataType, IdxType>::GenerateDataset(nrow, ncol, sparsity, 0.0, 10.0);
        const SparseMatrix query_set = SparseTestUtil<DataType, IdxType>::GenerateDataset(query_n, ncol, sparsity, 0.0, 10.0);
        const auto [gt_indices_list, gt_scores_list] = SparseTestUtil<DataType, IdxType>::GenerateGroundtruth(dataset, query_set, topk, false);

        String save_path = String(tmp_data_path()) + "/bmindex_test1.index";

        auto test_query = [&](const BMPAlg &index) {
            {
                SparseMatrixIter iter(query_set);
                SparseVecRef vec = iter.val();
                auto [indices, scores] = index.SearchKnn(vec, 0 /*topk*/, options);
                EXPECT_EQ(indices.size(), 0);
                EXPECT_EQ(scores.size(), 0);
            }
            u32 hit_all = 0;
            u32 total_all = 0;
            for (SparseMatrixIter iter(query_set); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();

                auto [indices, scores] = index.SearchKnn(vec, topk, options);

                u32 query_id = iter.row_id();
                const i32 *gt_indices = gt_indices_list.get() + query_id * topk;
                const DataType *gt_scores = gt_scores_list.get() + query_id * topk;

                auto [hit, total] = SparseTestUtil<DataType, IdxType>::CheckApproximateKnn(gt_indices, gt_scores, topk, indices, scores);
                hit_all += hit;
                total_all += total;

                // SparseTestUtil<DataType, IdxType>::PrintQuery(query_id, gt_indices, gt_scores, gt_size, indices, scores);
                // std::cout << fmt::format("accuracy: {}\n", (f32)hit / total);
            }
            //            std::cout << fmt::format("hit: {}, total: {}\n", hit_all, total_all);
            if (hit_all < total_all * accuracy_all) {
                EXPECT_TRUE(false);
            }
        };
        {
            BMPAlg index(ncol, block_size);
            for (SparseMatrixIter iter(dataset); iter.HasNext(); iter.Next()) {
                SparseVecRef vec = iter.val();
                auto row_id = iter.row_id();
                index.AddDoc(vec, row_id);
            }

            BMPOptimizeOptions optimize_options{.topk_ = static_cast<i32>(topk)};
            test_query(index);
            index.Optimize(optimize_options);
            test_query(index);

            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            index.Save(*file_handle);
        }
        {
            auto [file_handle, status] = VirtualStore::Open(save_path, FileAccessMode::kRead);
            if (!status.ok()) {
                UnrecoverableError(fmt::format("Failed to open file: {}", save_path));
            }
            auto index = BMPAlg::Load(*file_handle);

            test_query(index);
        }
    }
};

TEST_F(BMPIndexTest, test1) {
    {
        u32 block_size = 8;
        TestFunc<f32, i32, BMPCompressType::kCompressed>(block_size);
    }
    {
        u32 block_size = 64;
        TestFunc<f32, i32, BMPCompressType::kRaw>(block_size);
    }
    {
        u32 block_size = 8;
        TestFunc<f32, i16, BMPCompressType::kCompressed>(block_size);
    }
    {
        u32 block_size = 8;
        TestFunc<f64, i32, BMPCompressType::kCompressed>(block_size);
    }
}

TEST_F(BMPIndexTest, test2) {
    using BMPAlg = BMPAlg<f32, i32, BMPCompressType::kCompressed>;

    u32 ncol = 2;
    u32 block_size = 2;

    u32 topk = 5;

    BmpSearchOptions options;
    options.use_lock_ = false;

    Vector<i32> query_idx = {0, 1};
    Vector<f32> query_data = {1.0, 1.0};
    SparseVecRef query(query_idx.size(), query_idx.data(), query_data.data());

    Vector<i32> vec1_idx = {0};
    Vector<f32> vec1_data = {3.0};
    SparseVecRef vec1(vec1_idx.size(), vec1_idx.data(), vec1_data.data());

    Vector<i32> vec2_idx = {1};
    Vector<f32> vec2_data = {1.0};
    SparseVecRef vec2(vec2_idx.size(), vec2_idx.data(), vec2_data.data());

    Vector<i32> vec3_idx = {0, 1};
    Vector<f32> vec3_data = {1.0, 1.0};
    SparseVecRef vec3(vec3_idx.size(), vec3_idx.data(), vec3_data.data());

    BMPAlg index(ncol, block_size);
    index.AddDoc(vec1, 0);
    index.AddDoc(vec2, 1);
    index.AddDoc(vec3, 2);
    index.AddDoc(vec3, 3);
    index.AddDoc(vec3, 4);
    index.AddDoc(vec3, 5);

    [[maybe_unused]] auto [indices, scores] = index.SearchKnn(query, topk, options);
    ASSERT_EQ(indices.size(), topk);
    ASSERT_EQ(indices[0], 0);
    ASSERT_EQ(indices[1], 2);
    ASSERT_EQ(indices[2], 3);
    ASSERT_EQ(indices[3], 4);
    ASSERT_EQ(indices[4], 5);
    ASSERT_EQ(scores.size(), topk);
    ASSERT_EQ(scores[0], 3.0);
    ASSERT_EQ(scores[1], 2.0);
    ASSERT_EQ(scores[2], 2.0);
    ASSERT_EQ(scores[3], 2.0);
    ASSERT_EQ(scores[4], 2.0);
}
