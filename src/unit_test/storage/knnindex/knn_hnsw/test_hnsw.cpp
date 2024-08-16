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
#include <gtest/gtest.h>
#include <thread>

import stl;
import hnsw_alg;
import file_system;
import file_system_type;
import local_file_system;
import file_system_type;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
import data_store;
#pragma clang diagnostic pop

import dist_func_l2;
import dist_func_ip;
import dist_func_cos;
import vec_store_type;
import hnsw_common;
import infinity_exception;

using namespace infinity;

class HnswAlgTest : public BaseTest {
public:
    using LabelT = u64;

    const std::string save_dir_ = GetFullTmpDir();

    template <typename Hnsw>
    void TestSimple() {

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

        auto test_func = [&](auto &hnsw_index) {
            // std::fstream os("./tmp/dump.txt");
            // hnsw_index->Dump(os);
            // os.flush();
            hnsw_index->Check();

            KnnSearchOption search_option{.ef_ = 10};
            int correct = 0;
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                auto result = hnsw_index->KnnSearchSorted(query, 1, search_option);
                if (result[0].second == (LabelT)i) {
                    ++correct;
                }
            }
            float correct_rate = float(correct) / element_size;
            // std::printf("correct rage: %f\n", correct_rate);
            EXPECT_GE(correct_rate, 0.95);
        };

        LocalFileSystem fs;
        {
            auto hnsw_index = Hnsw::Make(chunk_size, max_chunk_n, dim, M, ef_construction);
            auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
            hnsw_index->InsertVecs(std::move(iter));

            test_func(hnsw_index);

            u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            auto [file_handler, status] = fs.OpenFile(save_dir_ + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            hnsw_index->Save(*file_handler);
        }

        {
            u8 file_flags = FileFlags::READ_FLAG;
            auto [file_handler, status] = fs.OpenFile(save_dir_ + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            auto hnsw_index = Hnsw::Load(*file_handler);

            test_func(hnsw_index);
        }
    }

    template <typename Hnsw, typename CompressedHnsw>
    void TestCompress() {
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

        auto test_func = [&](auto &hnsw_index) {
            hnsw_index->Check();

            KnnSearchOption search_option{.ef_ = 10};

            int correct = 0;
            for (int i = 0; i < element_size; ++i) {
                const float *query = data.get() + i * dim;
                auto result = hnsw_index->KnnSearchSorted(query, 1, search_option);
                if (result[0].second == (LabelT)i) {
                    ++correct;
                }
            }
            float correct_rate = float(correct) / element_size;
            // std::printf("correct rage: %f\n", correct_rate);
            EXPECT_GE(correct_rate, 0.95);
        };

        LocalFileSystem fs;
        {
            auto hnsw_index = Hnsw::Make(chunk_size, max_chunk_n, dim, M, ef_construction);

            auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
            hnsw_index->InsertVecs(std::move(iter));
            {
                // std::fstream os("./tmp/dump_1.txt", std::fstream::out);
                // hnsw_index->Dump(os);
            }
            auto compress_hnsw = std::move(*hnsw_index).CompressToLVQ();
            {
                // std::fstream os("./tmp/dump_2.txt", std::fstream::out);
                // compress_hnsw->Dump(os);
            }
            test_func(compress_hnsw);

            u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
            auto [file_handler, status] = fs.OpenFile(save_dir_ + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            compress_hnsw->Save(*file_handler);
        }
        {
            u8 file_flags = FileFlags::READ_FLAG;
            auto [file_handler, status] = fs.OpenFile(save_dir_ + "/test_hnsw.bin", file_flags, FileLockType::kNoLock);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }

            auto compress_hnsw = CompressedHnsw::Load(*file_handler);

            test_func(compress_hnsw);
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

        std::atomic<bool> starve = false;
        std::shared_mutex opt_mtx;

        auto SharedOptLck = [&]() {
            if (starve.load()) {
                starve.wait(true);
            }
            return std::shared_lock(opt_mtx);
        };
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
        auto UniqueOptLck = [&]() {
            bool old = false;
            bool success = starve.compare_exchange_strong(old, true);
            assert(success);
            auto ret = std::unique_lock(opt_mtx);
            starve.store(false);
            starve.notify_all();
            return ret;
        };
#pragma clang diagnostic pop

        auto write_thread = std::thread([&] {
            int start_i = 0, end_i = 0;
            {
                auto w_lck = UniqueOptLck();
                HnswInsertConfig config;
                config.optimize_ = true;
                auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size / 2);
                std::tie(start_i, end_i) = hnsw_index->StoreData(std::move(iter));
            }
            {
                auto write_thread2 = std::thread([&] {
                    int insert_n = element_size - element_size / 2;
                    for (int i = element_size / 2; i < element_size; ++i) {
                        DenseVectorIter<float, LabelT> iter(data.get(), dim, 1 /*insert_n*/);
                        hnsw_index->InsertVecs(std::move(iter));
                        if ((i + 1) % (insert_n / 4) == 0) {
                            auto w_lck = UniqueOptLck();
                            hnsw_index->Optimize();
                        }
                    }
                });

                write_thread2.detach();

                std::atomic<i32> idx = start_i;
                std::vector<std::thread> worker_threads;
                for (int i = 0; i < 4; ++i) {
                    worker_threads.emplace_back([&] {
                        while (true) {
                            i32 i = idx.fetch_add(1);
                            if (i >= end_i) {
                                break;
                            }
                            auto r_lck = SharedOptLck();
                            hnsw_index->Build(i);
                        }
                    });
                }
                for (int i = 0; i < 4; ++i) {
                    worker_threads[i].join();
                }
            }
            stop.store(true);
        });
        std::vector<std::thread> read_threads;
        for (int j = 0; j < 4; ++j) {
            read_threads.emplace_back([&] {
                while (stop.load() == false) {
                    for (int i = 0; i < element_size; ++i) {
                        const float *query = data.get() + i * dim;
                        auto r_lck = SharedOptLck();
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
    using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
    TestSimple<Hnsw>();
}

TEST_F(HnswAlgTest, test2) {
    using Hnsw = KnnHnsw<LVQL2VecStoreType<float, int8_t>, LabelT>;
    TestSimple<Hnsw>();
}

TEST_F(HnswAlgTest, test3) {
    using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
    TestParallel<Hnsw>();
}

TEST_F(HnswAlgTest, test4) {
    using Hnsw = KnnHnsw<LVQL2VecStoreType<float, int8_t>, LabelT>;
    TestParallel<Hnsw>();
}

TEST_F(HnswAlgTest, test5) {
    using Hnsw = KnnHnsw<PlainCosVecStoreType<float>, LabelT>;
    TestSimple<Hnsw>();
}

TEST_F(HnswAlgTest, test6) {
    using Hnsw = KnnHnsw<PlainL2VecStoreType<float>, LabelT>;
    using CompressedHnsw = KnnHnsw<LVQL2VecStoreType<float, int8_t>, LabelT>;
    TestCompress<Hnsw, CompressedHnsw>();
}
