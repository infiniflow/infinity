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

#include <thread>
#include "gtest/gtest.h"

import stl;
import base_test;
import column_def;
import embedding_info;
import logical_type;
import internal_types;
import data_type;

import hnsw_alg;
import hnsw_handler;
import index_hnsw;
import index_base;

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
import virtual_store;
import local_file_handle;

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
        kBuildBucketSize = 1024;

        index_name = MakeShared<String>("index_name");
        filename = "filename";
        column_names = {"col_name"};
        metric_type = MetricType::kMetricL2;
        encode_type = HnswEncodeType::kPlain;
        build_type = HnswBuildType::kPlain;

        filepath = save_dir_ + "/test_hnsw.bin";
    }

    UniquePtr<IndexHnsw> MakeIndexHnsw(bool compress = false) {
        HnswEncodeType tmp_encode_type = compress ? HnswEncodeType::kLVQ : encode_type;
        return MakeUnique<IndexHnsw>(index_name, nullptr, filename,
                                column_names, metric_type, tmp_encode_type,
                                build_type, M, ef_construction, chunk_size, None);
    }

    UniquePtr<ColumnDef> MakeColumnDef() {
        auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, dim);
        auto data_type = MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo);
        return MakeUnique<ColumnDef>(0, data_type, column_names[0], std::set<ConstraintType>());
    }

protected:
    SizeT dim;
    SizeT M;
    SizeT ef_construction;
    SizeT chunk_size;
    SizeT max_chunk_n;
    SizeT element_size;
    SizeT kBuildBucketSize;

    SharedPtr<String> index_name;
    String filename;
    Vector<String> column_names;
    MetricType metric_type;
    HnswEncodeType encode_type;
    HnswBuildType build_type;

    String filepath;
    const std::string save_dir_ = GetFullTmpDir();
};

TEST_F(HnswHandlerTest, test_memory) {
    /// generate random dataset
    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;

    auto data = MakeUnique<float[]>(dim * element_size);
    for (SizeT i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto test_search = [&](HnswHandler *hnsw_handler) {
        hnsw_handler->Check();
        KnnSearchOption search_option{.ef_ = 10};
        SizeT correct = 0;
        for (SizeT i = 0; i < element_size; ++i) {
            const auto *query = data.get() + i * dim;
            auto [result_n, d_ptr, v_ptr] = hnsw_handler->SearchIndex<float, LabelT>(query, 1, search_option);
            Vector<Pair<float, LabelT>> result(result_n);
            for (SizeT i = 0; i < result_n; ++i) {
                result[i] = {d_ptr[i], hnsw_handler->GetLabel<LabelT>(v_ptr[i])};
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
    };

    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    {
        /// get HnswHandler
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
        hnsw_handler->InsertVecs(std::move(iter), kBuildBucketSize);

        /// test interface
        hnsw_handler->GetType();
        auto [mem_usage, vec_num] = hnsw_handler->GetInfo();
        printf("hnsw_handler::GetInfo() -> %lu, %lu\n", mem_usage, vec_num);
        printf("hnsw_handler::GetSizeInBytes() -> %lu\n", hnsw_handler->GetSizeInBytes());
        test_search(hnsw_handler.get());

        /// test save
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kWrite);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        hnsw_handler->SaveToPtr(*file_handle);
    }

    /// test load
    {
        SizeT file_size = VirtualStore::GetFileSize(filepath);
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());

        hnsw_handler->LoadFromPtr(*file_handle, file_size);

        test_search(hnsw_handler.get());
    }
}

TEST_F(HnswHandlerTest, test_compress) {
    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;

    auto data = MakeUnique<float[]>(dim * element_size);
    for (SizeT i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto test_search = [&](auto &hnsw_handler) {
        hnsw_handler->Check();

        KnnSearchOption search_option{.ef_ = 10};

        int correct = 0;
        for (SizeT i = 0; i < element_size; ++i) {
            const float *query = data.get() + i * dim;
            auto [result_n, d_ptr, v_ptr] = hnsw_handler->template SearchIndex<float, LabelT>(query, 1, search_option);
            Vector<Pair<float, LabelT>> result(result_n);
            for (SizeT i = 0; i < result_n; ++i) {
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
    };

    {
        auto index_hnsw = MakeIndexHnsw();
        auto column_def = MakeColumnDef();

        /// get HnswHandler
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());
        auto iter = DenseVectorIter<float, LabelT>(data.get(), dim, element_size);
        hnsw_handler->InsertVecs(std::move(iter), kBuildBucketSize);

        hnsw_handler->CompressToLVQ();

        test_search(hnsw_handler);

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

        /// get HnswHandler
        auto hnsw_handler = HnswHandler::Make(index_hnsw.get(), column_def.get());

        SizeT file_size = VirtualStore::GetFileSize(filepath);
        auto [file_handle, status] = VirtualStore::Open(filepath, FileAccessMode::kRead);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        hnsw_handler->LoadFromPtr(*file_handle, file_size);

        test_search(hnsw_handler);
    }
}
