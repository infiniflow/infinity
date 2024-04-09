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
#include <fstream>
#include <thread>

import stl;
import hnsw_alg;
import file_system;
import file_system_type;
import local_file_system;
import file_system_type;
import data_store;
import dist_func_l2;
import dist_func_ip;
import compilation_config;
import vec_store_type;
import hnsw_common;

using namespace infinity;

class HnswAlgTest : public BaseTest {
public:
    using LabelT = u64;

    template <typename Hnsw>
    void TestSimple() {
        String save_dir = tmp_data_path();

        int dim = 16;
        int M = 8;
        int ef_construction = 200;
        int chunk_size = 128;
        int max_chunk_n = 10;
        int element_size = max_chunk_n * chunk_size;

        std::mt19937 rng;
        rng.seed(0);
        std::uniform_real_distribution<float> distrib_real;

        auto data = MakeUnique<float[]>(dim * element_size);
        for (int i = 0; i < dim * element_size; ++i) {
            data[i] = distrib_real(rng);
        }

        LocalFileSystem fs;
        {
            auto hnsw_index = Hnsw::Make(chunk_size, max_chunk_n, dim, M, ef_construction);

            hnsw_index->InsertVecsRaw(data.get(), element_size);
            // std::ofstream os("tmp/dump.txt");
            // hnsw_index->Dump(os);
            hnsw_index->Check();

            hnsw_index->SetEf(10);
            int correct = 0;
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                auto result = hnsw_index->KnnSearchSorted(query, 1);
                if (result[0].second == (LabelT)i) {
                    ++correct;
                }
            }
            float correct_rate = float(correct) / element_size;
            // std::printf("correct rage: %f\n", correct_rate);
            EXPECT_GE(correct_rate, 0.95);

            u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            UniquePtr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);
            hnsw_index->Save(*file_handler);
            file_handler->Close();
        }

        {
            u8 file_flags = FileFlags::READ_FLAG;
            UniquePtr<FileHandler> file_handler = fs.OpenFile(save_dir + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);

            auto hnsw_index = Hnsw::Load(*file_handler);
            hnsw_index->SetEf(10);

            // std::ofstream os("tmp/dump2.txt");
            // hnsw_index->Dump(os);
            hnsw_index->Check();
            int correct = 0;
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                auto result = hnsw_index->KnnSearchSorted(query, 1);
                if (result[0].second == (LabelT)i) {
                    ++correct;
                }
            }
            float correct_rate = float(correct) / element_size;
            // std::printf("correct rage: %f\n", correct_rate);
            EXPECT_GE(correct_rate, 0.95);

            file_handler->Close();
        }
    }

    template <typename Hnsw>
    void TestParallel() {
        int dim = 16;
        int M = 8;
        int ef_construction = 200;
        int chunk_size = 128;
        int max_chunk_n = 10;

        int element_size = max_chunk_n * chunk_size;

        std::mt19937 rng;
        rng.seed(0);
        std::uniform_real_distribution<float> distrib_real;

        auto data = MakeUnique<float[]>(dim * element_size);
        for (int i = 0; i < dim * element_size; ++i) {
            data[i] = distrib_real(rng);
        }

        LocalFileSystem fs;
        auto hnsw_index = Hnsw::Make(chunk_size, max_chunk_n, dim, M, ef_construction);

        std::atomic<bool> stop = false;
        auto write_thread = std::thread([&] {
            HnswInsertConfig config;
            config.optimize_ = true;

            auto [start_i, end_i] = hnsw_index->StoreDataRaw(data.get(), element_size / 2, 0 /*offset*/, config);
            {
                std::atomic<i32> idx = start_i;
                std::vector<std::jthread> worker_threads;
                for (int i = 0; i < 4; ++i) {
                    worker_threads.emplace_back([&] {
                        while (true) {
                            i32 i = idx.fetch_add(1);
                            if (i >= end_i) {
                                break;
                            }
                            hnsw_index->Build(i);
                        }
                    });
                }
            }
            for (int i = element_size / 2; i < element_size; ++i) {
                hnsw_index->InsertVecsRaw(data.get() + i * dim, 1 /*offset*/, i);
            }
            stop.store(true);
        });
        std::vector<std::thread> read_threads;
        for (int i = 0; i < 4; ++i) {
            read_threads.emplace_back([&] {
                while (stop.load() == false) {
                    for (int i = 0; i < element_size; ++i) {
                        const float *query = data.get() + i * dim;
                        auto result = hnsw_index->KnnSearchSorted(query, 1);
                        // if (!result.empty()) {
                        //     EXPECT_EQ(result[0].second, (LabelT)i);
                        // }
                    }
                }
            });
        }
        write_thread.join();
        for (auto &t : read_threads) {
            t.join();
        }
    }
};

TEST_F(HnswAlgTest, test1) {
    // NOTE: inner product correct rate is not 1. (the vector and itself's distance is not the smallest)
    {
        using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
        TestSimple<Hnsw>();
    }
    {
        using Hnsw = KnnHnsw<LVQL2VecStoreType<float, int8_t>, LabelT>;
        TestSimple<Hnsw>();
    }
}

TEST_F(HnswAlgTest, test2) {
    {
        using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
        TestParallel<Hnsw>();
    }
    {
        using Hnsw = KnnHnsw<LVQL2VecStoreType<float, int8_t>, LabelT>;
        TestParallel<Hnsw>();
    }
}
