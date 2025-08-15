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

#ifdef CI
#include "gtest/gtest.h"
#include <thread>
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"
#include <thread>

module infinity_core:ut.test_hnsw_handler;

import :ut.base_test;
import :stl;
import :hnsw_alg;
import :hnsw_handler;
import :index_hnsw;
import :index_base;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
import :data_store;
#pragma clang diagnostic pop
import :dist_func_l2;
import :dist_func_ip;
import :dist_func_cos;
import :vec_store_type;
import :hnsw_common;
import :infinity_exception;
import :virtual_store;
import :local_file_handle;
#endif

import column_def;
import embedding_info;
import logical_type;
import internal_types;
import data_type;

using namespace infinity;

class HnswHandlerTest : public BaseTest {
public:
    using LabelT = u32;
    using QueryDataType = unsigned char;

    void SetUp() override {
        dim = 16;
        M = 8;
        ef_construction = 200;
        chunk_size = 128;
        max_chunk_n = 16;
        element_size = max_chunk_n * chunk_size;

        index_name = std::make_shared<String>("index_name");
        filename = "filename";
        column_names = {"col_name"};
        metric_type = MetricType::kMetricL2;
        encode_type = HnswEncodeType::kPlain;
        build_type = HnswBuildType::kPlain;

        filepath = save_dir_ + "/test_hnsw.bin";

        std::mt19937 rng;
        rng.seed(0);
        std::uniform_real_distribution<float> distrib_real;
        data = std::make_unique<float[]>(dim * element_size);
        for (size_t i = 0; i < dim * element_size; ++i) {
            data[i] = distrib_real(rng);
        }
    }

    std::unique_ptr<IndexHnsw> MakeIndexHnsw(bool compress = false) {
        HnswEncodeType tmp_encode_type = compress ? HnswEncodeType::kLVQ : encode_type;
        return std::make_unique<
            IndexHnsw>(index_name, nullptr, filename, column_names, metric_type, tmp_encode_type, build_type, M, ef_construction, chunk_size, None);
    }

    std::unique_ptr<ColumnDef> MakeColumnDef() {
        auto embeddingInfo = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, dim);
        auto data_type = std::make_shared<DataType>(LogicalType::kEmbedding, embeddingInfo);
        return std::make_unique<ColumnDef>(0, data_type, column_names[0], std::set<ConstraintType>());
    }

    void SearchHnswHandler(HnswHandler *hnsw_handler) {
        hnsw_handler->Check();

        KnnSearchOption search_option{.ef_ = 10};

        int correct = 0;
        for (size_t i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            auto [result_n, d_ptr, v_ptr] = hnsw_handler->template SearchIndex<float, LabelT>(query, 1, search_option);
            Vector<Pair<float, LabelT>> result(result_n);
            for (size_t i = 0; i < result_n; ++i) {
                result[i] = {d_ptr[i], hnsw_handler->template GetLabel<LabelT>(v_ptr[i])};
            }
            std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
            if (result.empty()) {
                continue;
            }
            if (result[0].second == (LabelT)i) {
                ++correct;
            }
        }
        float correct_rate = float(correct) / element_size;
        std::printf("correct rage: %f\n", correct_rate);
        EXPECT_GE(correct_rate, 0.95);
    }

protected:
    size_t dim;
    size_t M;
    size_t ef_construction;
    size_t chunk_size;
    size_t max_chunk_n;
    size_t element_size;

    std::shared_ptr<String> index_name;
    String filename;
    Vector<String> column_names;
    MetricType metric_type;
    HnswEncodeType encode_type;
    HnswBuildType build_type;

    std::unique_ptr<float[]> data = nullptr;
    String filepath;
    const std::string save_dir_ = GetFullTmpDir();
};

TEST_F(HnswHandlerTest, test_memory) {
    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    {
        /// get HnswHandler
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
        hnsw_handler->InsertVecs(std::move(iter));

        /// test interface
        auto [mem_usage, vec_num] = hnsw_handler->GetInfo();
        printf("hnsw_handler::GetInfo() -> %lu, %lu\n", (std::size_t)mem_usage, (std::size_t)vec_num);
        printf("hnsw_handler::GetSizeInBytes() -> %lu\n", (std::size_t)hnsw_handler->GetSizeInBytes());
        SearchHnswHandler(hnsw_handler.get());

        /// test save
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->SaveToPtr(*file_handle);
    }

    /// test load
    {
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        size_t file_size = VirtualStore::GetFileSize(filepath);
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->LoadFromPtr(*file_handle, file_size);

        SearchHnswHandler(hnsw_handler.get());
    }
}

TEST_F(HnswHandlerTest, test_compress) {
    /// build in memory
    {
        auto index_hnsw = MakeIndexHnsw();
        auto column_def = MakeColumnDef();
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
        hnsw_handler->InsertVecs(std::move(iter));

        hnsw_handler->CompressToLVQ();

        SearchHnswHandler(hnsw_handler.get());

        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->SaveToPtr(*file_handle);
    }

    /// load by compress
    {
        auto index_hnsw = MakeIndexHnsw(true);
        auto column_def = MakeColumnDef();
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        size_t file_size = VirtualStore::GetFileSize(filepath);
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->LoadFromPtr(*file_handle, file_size);

        SearchHnswHandler(hnsw_handler.get());
    }
}

TEST_F(HnswHandlerTest, test_load) {
    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();
    /// save index file
    {
        /// get HnswHandler
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
        hnsw_handler->InsertVecs(std::move(iter));

        /// save
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->SaveToPtr(*file_handle);
    }
    /// load by file_handle
    {
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());

        size_t file_size = VirtualStore::GetFileSize(filepath);
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->LoadFromPtr(*file_handle, file_size);

        SearchHnswHandler(hnsw_handler.get());
    }
    /// load by mmap
    {
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get(), false);
        size_t file_size = VirtualStore::GetFileSize(filepath);
#define USE_MMAP
#ifdef USE_MMAP
        unsigned char *data_ptr = nullptr;
        int ret = VirtualStore::MmapFile(filepath, data_ptr, file_size);
        if (ret < 0) {
            UnrecoverableError("mmap failed");
        }
        const char *ptr = reinterpret_cast<const char *>(data_ptr);
#else
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        auto buffer = std::make_unique<char[]>(file_size);
        file_handle->Read(buffer.get(), file_size);
        const char *ptr = buffer.get();
#endif
        hnsw_handler->LoadFromPtr(ptr, file_size);

        SearchHnswHandler(hnsw_handler.get());

#ifdef USE_MMAP
        VirtualStore::MunmapFile(filepath);
#endif
    }
}

TEST_F(HnswHandlerTest, test_parallel) {
    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();
    auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());

    std::atomic<int> total = 0;
    std::atomic<int> res = 0;
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
        i32 start_i = 0, end_i = 0;
        {
            auto w_lck = UniqueOptLck();
            HnswInsertConfig config;
            config.optimize_ = true;
            std::tie(start_i, end_i) = hnsw_handler->StoreData<LabelT>(data.get(), dim, element_size / 2);
        }
        {
            auto write_thread2 = std::thread([&] {
                size_t insert_n = element_size - element_size / 2;
                for (size_t i = element_size / 2; i < element_size; ++i) {
                    DenseVectorIter<float, LabelT> iter(data.get(), dim, 1 /*insert_n*/);
                    hnsw_handler->InsertVecs(std::move(iter));
                    if ((i + 1) % (insert_n / 4) == 0) {
                        auto w_lck = UniqueOptLck();
                        hnsw_handler->Optimize();
                    }
                }
            });

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
                        hnsw_handler->Build(i);
                    }
                });
            }
            for (int i = 0; i < 4; ++i) {
                worker_threads[i].join();
            }
            write_thread2.join();
        }
        stop.store(true);
    });
    std::vector<std::thread> read_threads;
    for (int j = 0; j < 4; ++j) {
        read_threads.emplace_back([&] {
            while (stop.load() == false) {
                for (size_t i = 0; i < element_size; ++i) {
                    const float *query = data.get() + i * dim;
                    auto r_lck = SharedOptLck();
                    auto [result_n, d_ptr, v_ptr] = hnsw_handler->template SearchIndex<float, LabelT>(query, 1);
                    Vector<Pair<float, LabelT>> result(result_n);
                    for (size_t k = 0; k < result_n; ++k) {
                        result[k] = {d_ptr[k], hnsw_handler->template GetLabel<LabelT>(v_ptr[k])};
                    }
                    std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
                    if (!result.empty()) {
                        ++res;
                    }
                    ++total;
                }
            }
        });
    }
    write_thread.join();
    for (auto &t : read_threads) {
        t.join();
    }
    float correct_rate = float(res) / total;
    std::printf("correct rage: %f\n", correct_rate);
    EXPECT_GE(correct_rate, 0.95);
}
