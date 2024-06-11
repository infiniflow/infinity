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

#include <random>

import local_file_system;
import file_system;
import file_system_type;
import dist_func_l2;
import data_store;
import vec_store_type;
import stl;
import infinity_exception;
import hnsw_common;

using namespace infinity;

class HnswLVQTest : public BaseTest {
public:
    void SetUp() override {
        system(("rm -rf " + file_dir_).c_str());
        system(("mkdir -p " + file_dir_).c_str());
    }

    void TearDown() override { system(("rm -rf " + file_dir_).c_str()); }

public:
    using LabelT = int;
    using VecStoreType = LVQL2VecStoreType<float, int8_t>;
    using DataStore = DataStore<VecStoreType, LabelT>;
    using LVQ8Data = typename VecStoreType::StoreType;

    static constexpr size_t dim_ = 16;
    static constexpr size_t vec_n_ = 32;
    const std::string file_dir_ = GetTmpDir();

    bool dump_ = false;

    void CheckStore(DataStore &store, const float *vecs) {
        std::cout.widen(8);
        std::cout.fill('0');
        const auto &vec_store_meta = store.vec_store_meta();
        const auto *mean = vec_store_meta.mean();
        for (size_t i = 0; i < store.cur_vec_num(); ++i) {
            const float *vec = vecs + dim_ * i;
            LVQ8Data lvq = store.GetVec(i);
            const auto *c = lvq->compress_vec_;
            auto res = std::make_unique<float[]>(dim_);
            for (size_t j = 0; j < dim_; ++j) {
                auto scale = lvq->scale_;
                auto bias = lvq->bias_;
                res[j] = scale * c[j] + bias + mean[j];
            }

            if (dump_) {
                std::cout << i << std::endl;
                std::cout << "LVQ:\t";
                for (size_t j = 0; j < 16; ++j) {
                    std::cout << res[j] << " ";
                }
                std::cout << std::endl;

                std::cout << "Old:\t";
                for (size_t j = 0; j < 16; ++j) {
                    std::cout << vec[j] << " ";
                }
                std::cout << std::endl;
            }

            for (size_t j = 0; j < dim_; ++j) {
                float error = std::abs((res[j] - vec[j]) / vec[j]);
                EXPECT_LE(error, 2e-2);
            }
        }
        if (dump_) {
            std::cout << "---------------" << std::endl;
        }
    }
};

TEST_F(HnswLVQTest, test1) {
    using namespace infinity;

    auto data = std::make_unique<float[]>(dim_ * vec_n_);

    // randomize the data from 0 to 1
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real(100, 200);
    for (size_t i = 0; i < dim_ * vec_n_; ++i) {
        data[i] = distrib_real(rng);
    }

    // dump_ = true;
    {
        auto lvq_store = DataStore::Make(vec_n_, 1 /*chunk_n*/, dim_, 0 /*Mmax0*/, 0 /*Mmax*/);
        auto iter = DenseVectorIter<float, LabelT>(data.get(), dim_, vec_n_);
        auto [start_i, end_i] = lvq_store.OptAddVec(std::move(iter));
        EXPECT_EQ(start_i, 0u);
        EXPECT_EQ(end_i, vec_n_);
        CheckStore(lvq_store, data.get());
    }

    {
        size_t idx = 0;
        auto lvq_store = DataStore::Make(vec_n_, 1 /*chunk_n*/, dim_, 0 /*Mmax0*/, 0 /*Mmax*/);
        {
            auto iter = DenseVectorIter<float, LabelT>(data.get(), dim_, vec_n_ / 2);
            auto [start_i, end_i] = lvq_store.OptAddVec(std::move(iter));
            EXPECT_EQ(start_i, 0u);
            EXPECT_EQ(end_i, vec_n_ / 2);
            idx = end_i;
        }

        {
            auto iter = DenseVectorIter<float, LabelT>(data.get() + idx * dim_, dim_, vec_n_ - idx);
            auto [start_i, end_i] = lvq_store.AddVec(std::move(iter));
            EXPECT_EQ(start_i, vec_n_ / 2);
            EXPECT_EQ(end_i, vec_n_);
        }
        CheckStore(lvq_store, data.get());
        lvq_store.Optimize();
        CheckStore(lvq_store, data.get());
    }

    {
        size_t idx = 0;
        auto lvq_store = DataStore::Make(vec_n_, 1 /*chunk_n*/, dim_, 0 /*Mmax0*/, 0 /*Mmax*/);
        {
            auto iter = DenseVectorIter<float, LabelT>(data.get(), dim_, vec_n_ / 2);
            auto [start_i, end_i] = lvq_store.AddVec(std::move(iter));
            EXPECT_EQ(start_i, 0u);
            EXPECT_EQ(end_i, vec_n_ / 2);
            idx = end_i;
        }

        {
            auto iter = DenseVectorIter<float, LabelT>(data.get() + idx * dim_, dim_, vec_n_ - idx);
            auto [start_i, end_i] = lvq_store.OptAddVec(std::move(iter));
            EXPECT_EQ(start_i, vec_n_ / 2);
            EXPECT_EQ(end_i, vec_n_);
        }
        CheckStore(lvq_store, data.get());
    }

    {
        auto lvq_store = DataStore::Make(vec_n_, 1 /*chunk_n*/, dim_, 0 /*Mmax0*/, 0 /*Mmax*/);
        {
            for (SizeT i = 0; i < vec_n_; ++i) {
                auto iter = DenseVectorIter<float, LabelT>(data.get() + i * dim_, dim_, 1);
                auto [start_i, end_i] = lvq_store.OptAddVec(std::move(iter));
                EXPECT_EQ(start_i, i);
                EXPECT_EQ(end_i, i + 1);
                CheckStore(lvq_store, data.get());
            }
        }
    }

    {
        std::string file_path = file_dir_ + "/lvq_store1.bin";

        LocalFileSystem fs;
        fs.CleanupDirectory(file_dir_);

        {
            uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            auto [file_handler, status] = fs.OpenFile(file_path, file_flags, FileLockType::kWriteLock);
            if(!status.ok()) {
                UnrecoverableError(status.message());
            }

            auto lvq_store = DataStore::Make(vec_n_, 1 /*chunk_n*/, dim_, 0 /*Mmax0*/, 0 /*Mmax*/);
            auto iter = DenseVectorIter<float, LabelT>(data.get(), dim_, vec_n_ / 2);
            auto [start_i, end_i] = lvq_store.OptAddVec(std::move(iter));
            EXPECT_EQ(start_i, 0u);
            EXPECT_EQ(end_i, vec_n_ / 2);
            auto iter2 = DenseVectorIter<float, LabelT>(data.get() + vec_n_ / 2 * dim_, dim_, vec_n_ - vec_n_ / 2);
            lvq_store.AddVec(std::move(iter2));

            lvq_store.Save(*file_handler);
        }
        {
            uint8_t file_flags = FileFlags::READ_FLAG;
            auto [file_handler, status] = fs.OpenFile(file_path, file_flags, FileLockType::kReadLock);
            if(!status.ok()) {
                UnrecoverableError(status.message());
            }
            auto lvq_store = DataStore::Load(*file_handler);

            CheckStore(lvq_store, data.get());
        }
    }
}