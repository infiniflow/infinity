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
#include "unit_test/gtest_expand.h"

import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_lsg_build;

import :ut.base_test;
import :hnsw_lsg_builder;
import :index_hnsw;
import :hnsw_common;
import :index_base;
import :hnsw_handler;
import :infinity_exception;
import :hnsw_alg;
#endif

import column_def;
import embedding_info;
import data_type;
import logical_type;
import internal_types;

using namespace infinity;

class LSGBuildTest : public BaseTest {
protected:
    void SetUp() override {
        M = 8;
        ef_construction = 200;
        block_size = 128;
        dim = 128;
        element_size = 8192;

        index_name = std::make_shared<std::string>("index_name");
        filename = "filename";
        column_names = {"col_name"};
        metric_type = MetricType::kMetricL2;
        encode_type = HnswEncodeType::kPlain;
        build_type = HnswBuildType::kLSG;
        lsg_config = LSGConfig();
    }

    std::unique_ptr<IndexHnsw> MakeIndexHnsw() {
        return std::make_unique<
            IndexHnsw>(index_name, nullptr, filename, column_names, metric_type, encode_type, build_type, M, ef_construction, block_size, lsg_config);
    }

    std::unique_ptr<ColumnDef> MakeColumnDef() {
        auto embeddingInfo = std::make_shared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, dim);
        auto data_type = std::make_shared<DataType>(LogicalType::kEmbedding, embeddingInfo);
        return std::make_unique<ColumnDef>(0, data_type, column_names[0], std::set<ConstraintType>());
    }

protected:
    size_t M;
    size_t ef_construction;
    size_t block_size;
    size_t dim;
    size_t element_size;

    std::shared_ptr<std::string> index_name;
    std::string filename;
    std::vector<std::string> column_names;
    MetricType metric_type;
    HnswEncodeType encode_type;
    HnswBuildType build_type;
    std::optional<LSGConfig> lsg_config;

    using LabelT = u32;
};

TEST_F(LSGBuildTest, test_avg) {
    element_size = 128;
    lsg_config->sample_ratio_ = 0.1;

    auto data = std::make_unique<float[]>(dim * element_size);
    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;
    for (size_t i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto iter = DenseVectorIter<f32, LabelT>(data.get(), dim, element_size);

    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def));

    auto avg = lsg_builder.GetLSAvg<decltype(iter), f32, f32>(std::move(iter), element_size, RowID(0, 0));

    auto avg_gt = std::make_unique<float[]>(element_size);
    {
        size_t ls_k = std::min(lsg_config->ls_k_, element_size);
        auto distances = std::make_unique<float[]>(element_size);
        for (size_t i = 0; i < element_size; ++i) {
            const float *v = data.get() + i * dim;
            for (size_t j = 0; j < element_size; ++j) {
                const float *v2 = data.get() + j * dim;
                float sum = 0;
                for (size_t k = 0; k < dim; ++k) {
                    float diff = v[k] - v2[k];
                    sum += diff * diff;
                }
                // float distance = std::sqrt(sum);
                // distances[j] = distance;
                distances[j] = sum;
            }
            std::sort(distances.get(), distances.get() + element_size);
            float distance_sum = 0;
            for (size_t j = 0; j < ls_k; ++j) {
                distance_sum += distances[j];
            }
            avg_gt[i] = distance_sum / ls_k;
        }
    }

    for (std::size_t i = 0; i < element_size; ++i) {
        printf("i: %zu, avg: %f, avg_gt: %f\n", i, avg[i], avg_gt[i]);
        ASSERT_GE(avg[i], avg_gt[i]);
    }
}

TEST_F(LSGBuildTest, test1) {
    dim = 16;
    lsg_config->sample_ratio_ = 0.1;

    auto data = std::make_unique<float[]>(dim * element_size);

    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;
    for (size_t i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def));

    auto iter = DenseVectorIter<f32, LabelT>(data.get(), dim, element_size);

    std::unique_ptr<HnswIndexInMem> hnsw_index = lsg_builder.MakeImplIter<decltype(iter), f32, f32>(std::move(iter), element_size, RowID(0, 0), false);

    u32 correct_count = 0;
    i32 topk = 1;
    KnnSearchOption search_option{.ef_ = size_t(topk) * 10};
    for (size_t i = 0; i < element_size; ++i) {
        const float *query = data.get() + i * dim;
        HnswHandlerPtr hnsw_handler = hnsw_index->get();
        auto [result_n, d_ptr, v_ptr] = hnsw_handler->SearchIndex<float, LabelT>(query, topk, search_option);
        std::vector<std::pair<f32, LabelT>> res(result_n);
        for (size_t i = 0; i < result_n; ++i) {
            res[i] = {d_ptr[i], hnsw_handler->GetLabel<LabelT>(v_ptr[i])};
        }
        std::sort(res.begin(), res.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        if (res.empty()) {
            continue;
        }
        LabelT label = res[0].second;
        if (label == static_cast<LabelT>(i)) {
            correct_count++;
        }
    }
    float correct_rate = float(correct_count) / element_size;
    printf("correct rate: %f\n", correct_rate);
    ASSERT_GE(correct_rate, 0.95);
}
