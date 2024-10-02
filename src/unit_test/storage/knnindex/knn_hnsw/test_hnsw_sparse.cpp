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

#include "gtest/gtest.h"
import base_test;

import stl;
import hnsw_alg;
import vec_store_type;
import file_system;
import local_file_system;
import hnsw_common;
import sparse_util;
import file_system_type;
import compilation_config;
import infinity_exception;
import third_party;
import sparse_test_util;
import virtual_store;
import local_file_handle;
import abstract_file_handle;

using namespace infinity;

class HnswSparseTest : public BaseTest {
protected:
    using LabelT = u64;
    using IdxT = i32;

    const std::string save_dir_ = GetFullTmpDir();

    template <typename Hnsw>
    void TestSimple() {
        int max_dim = 1600;
        float sparsity = 0.01;
        int M = 16;
        int ef_construction = 200;
        int chunk_size = 128;
        int max_chunk_n = 10;
        SizeT element_size = max_chunk_n * chunk_size;

        SparseMatrix dataset = SparseTestUtil<f32, i32>::GenerateDataset(element_size, max_dim, sparsity, 0, 1.0);
        auto [gt_idx, gt_score] = SparseTestUtil<f32, i32>::GenerateGroundtruth(dataset, dataset, 1);

        {
            auto hnsw_index = Hnsw::Make(chunk_size, max_chunk_n, max_dim, M, ef_construction);
            auto iter = SparseVectorIter<float, IdxT, LabelT>(dataset.indptr_.get(), dataset.indices_.get(), dataset.data_.get(), dataset.nrow_);
            hnsw_index->InsertVecs(std::move(iter));

            {
                Path dump_path = Path(tmp_data_path()) / "dump.txt";
                std::fstream ss(dump_path, std::fstream::out);
                if (!ss.is_open()) {
                    UnrecoverableError("Failed to open file");
                }
                hnsw_index->Dump(ss);
                hnsw_index->Check();
            }

            KnnSearchOption search_option{.ef_ = 50};
            for (SizeT i = 0; i < element_size; ++i) {
                SparseVecRef<f32, IdxT> query = dataset.at(i);
                if (gt_score[i] == 0.0 || query.nnz_ == 0) {
                    continue;
                }
                Vector<Pair<f32, LabelT>> res = hnsw_index->KnnSearchSorted(query, 1, search_option);
                //                if (int(res[0].second) != gt_idx[i]) {
                //                    std::cout << (fmt::format("{}, {}", res[0].second, gt_idx[i])) << std::endl;
                //                    std::cout << (fmt::format("{}, {}", -res[0].first, gt_score[i])) << std::endl;
                //                }
                // EXPECT_EQ(res[0].second, gt_idx[i]);
                // EXPECT_NEAR(-res[0].first, gt_score[i], 1e-5);
            }

            auto [file_handle, status] = LocalStore::Open(save_dir_ + "/test_hnsw_sparse.bin", FileAccessMode::kWrite);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            hnsw_index->Save(*file_handle);
            file_handle->Close();
        }
        {
            auto [file_handle, status] = LocalStore::Open(save_dir_ + "/test_hnsw_sparse.bin", FileAccessMode::kRead);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            auto hnsw_index = Hnsw::Load(*file_handle);
            KnnSearchOption search_option{.ef_ = 50};
            for (SizeT i = 0; i < element_size; ++i) {
                SparseVecRef<f32, IdxT> query = dataset.at(i);
                if (gt_score[i] == 0.0 || query.nnz_ == 0) {
                    continue;
                }
                Vector<Pair<f32, LabelT>> res = hnsw_index->KnnSearchSorted(query, 1, search_option);
                //                if (int(res[0].second) != gt_idx[i]) {
                //                    std::cout << (fmt::format("{}, {}", res[0].second, gt_idx[i])) << std::endl;
                //                    std::cout << (fmt::format("{}, {}", -res[0].first, gt_score[i])) << std::endl;
                //                }
                // EXPECT_EQ(res[0].second, gt_idx[i]);
                // EXPECT_NEAR(-res[0].first, gt_score[i], 1e-5);
            }
            file_handle->Close();
        }
    }

private:
    template <typename T>
    Vector<T> MakeRandom(T min, T max, SizeT num, std::mt19937 &rng) {
        std::uniform_real_distribution<float> distrib_real;
        HashSet<T> set;
        while (set.size() < num) {
            set.insert(distrib_real(rng) * (max - min) + min);
        }
        return Vector<T>(set.begin(), set.end());
    }
};

TEST_F(HnswSparseTest, test1) {
    using Hnsw = KnnHnsw<SparseIPVecStoreType<float, IdxT>, LabelT>;
    TestSimple<Hnsw>();
}
