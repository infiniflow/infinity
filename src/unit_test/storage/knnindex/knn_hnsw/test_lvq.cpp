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
#include <iostream>
#include <random>

import lvq_store;
import local_file_system;
import file_system;
import file_system_type;
import dist_func_l2;
import compilation_config;

using namespace infinity;

class HnswLVQTest : public BaseTest {
public:
    using LVQ8Store = LVQStore<float, int8_t, LVQL2Cache<float, int8_t>>;
    using LVQ8Data = LVQ8Store::StoreType;

    static constexpr size_t dim_ = 16;
    static constexpr size_t vec_n_ = 20;
    static constexpr size_t buffer_size_ = 4;
    const std::string file_dir_ = tmp_data_path();

    bool dump_ = false;

    void CheckStore(LVQ8Store &store, const float *vecs) {

        std::cout.widen(8);
        std::cout.fill('0');
        auto mean = store.GetMean();
        store.Compress();
        for (size_t i = 0; i < store.cur_vec_num(); ++i) {
            const float *vec = vecs + dim_ * i;
            LVQ8Data lvq = store.GetVec(i);
            auto c = lvq.GetCompressVec();
            auto res = std::make_unique<float[]>(dim_);
            for (size_t j = 0; j < dim_; ++j) {
                auto [scale, bias] = lvq.GetScalar();
                res[j] = scale * c[j] + bias + mean[j];
                float error = std::abs((res[j] - vec[j]) / vec[j]);
                EXPECT_LE(error, 1e-2);
            }

            if (!dump_) {
                continue;
            }
            std::cout << i << std::endl;
            std::cout << "LVQ:\t";
            for (size_t j = 0; j < 16; ++j) {
                std::cout << res[j] << " ";
            }
            std::cout << std::endl;

            std::cout << "Old:\t";
            for (size_t j = 0; j < 16; ++j) {
                std::cout << vec[i] << " ";
            }
            std::cout << std::endl;
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
        LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
        auto ret = lvq_store.AddVec(data.get(), vec_n_);
        EXPECT_NE(ret, LVQ8Store::ERR_IDX);
        CheckStore(lvq_store, data.get());
    }

    {
        size_t idx = 0;
        LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
        auto ret = lvq_store.AddVec(data.get(), vec_n_ / 2);
        EXPECT_NE(ret, LVQ8Store::ERR_IDX);
        idx += vec_n_ / 2;

        ret = lvq_store.AddVec(data.get() + idx * dim_, vec_n_ - idx);
        EXPECT_NE(ret, LVQ8Store::ERR_IDX);
        CheckStore(lvq_store, data.get());
    }

    {
        size_t idx = 0;
        LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
        auto ret = lvq_store.AddVec(data.get(), vec_n_ / 2);
        EXPECT_NE(ret, LVQ8Store::ERR_IDX);
        idx += vec_n_ / 2;

        for (size_t i = 0; i < buffer_size_ && idx < vec_n_; ++i) {
            ret = lvq_store.AddVec(data.get() + idx * dim_, 1);
            EXPECT_NE(ret, LVQ8Store::ERR_IDX);
            ++idx;
        }
        ret = lvq_store.AddVec(data.get() + idx * dim_, vec_n_ - idx);
        EXPECT_NE(ret, LVQ8Store::ERR_IDX);

        CheckStore(lvq_store, data.get());
    }

    {
        std::string file_path = file_dir_ + "/lvq_store1.bin";
        LocalFileSystem fs;
        if (fs.Exists(file_dir_)) {
            fs.DeleteDirectory(file_dir_);
        }
        fs.CreateDirectory(file_dir_);

        size_t idx = 0;
        {
            uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kWriteLock);

            LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
            auto ret = lvq_store.AddVec(data.get(), vec_n_ / 2);
            EXPECT_NE(ret, LVQ8Store::ERR_IDX);
            idx += vec_n_ / 2;

            lvq_store.Save(*file_handler);
        }
        {
            uint8_t file_flags = FileFlags::READ_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kReadLock);

            LVQ8Store lvq_store = LVQ8Store::Load(*file_handler, 0, buffer_size_);

            CheckStore(lvq_store, data.get());
        }
    }

    {
        std::string file_path = file_dir_ + "/lvq_store2.bin";
        LocalFileSystem fs;

        size_t idx = 0;
        {
            uint8_t file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kWriteLock);

            LVQ8Store lvq_store = LVQ8Store::Make(vec_n_, dim_, buffer_size_);
            auto ret = lvq_store.AddVec(data.get(), vec_n_ / 2);
            EXPECT_NE(ret, LVQ8Store::ERR_IDX);
            idx += vec_n_ / 2;

            for (size_t i = 0; i < buffer_size_ && idx < vec_n_; ++i) {
                ret = lvq_store.AddVec(data.get() + idx * dim_, 1);
                EXPECT_NE(ret, LVQ8Store::ERR_IDX);
                ++idx;
            }
            lvq_store.Save(*file_handler);
        }
        {
            uint8_t file_flags = FileFlags::READ_FLAG;
            std::unique_ptr<FileHandler> file_handler = fs.OpenFile(file_path, file_flags, FileLockType::kReadLock);

            LVQ8Store lvq_store = LVQ8Store::Load(*file_handler, 0, buffer_size_);

            auto ret = lvq_store.AddVec(data.get() + idx * dim_, vec_n_ - idx);
            EXPECT_NE(ret, LVQ8Store::ERR_IDX);

            CheckStore(lvq_store, data.get());
        }
    }
}