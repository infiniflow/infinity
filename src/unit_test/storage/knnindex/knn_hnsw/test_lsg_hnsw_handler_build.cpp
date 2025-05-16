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
#include <cstdio>
#include <random>

import stl;
import base_test;
import new_hnsw_lsg_builder;
import index_hnsw;
import column_def;
import embedding_info;
import data_type;
import hnsw_common;
import logical_type;
import index_base;
import internal_types;
import hnsw_handler;
import infinity_exception;
import hnsw_alg;

using namespace infinity;

class LSGHnswHandlerBuildTest : public BaseTest {
protected:
    void SetUp() override {
        M = 8;
        ef_construction = 200;
        block_size = 128;
        dim = 128;
        element_size = 8192;

        index_name = MakeShared<String>("index_name");
        filename = "filename";
        column_names = {"col_name"};
        metric_type = MetricType::kMetricL2;
        encode_type = HnswEncodeType::kPlain;
        build_type = HnswBuildType::kLSG;
        lsg_config = LSGConfig();
    }

    UniquePtr<IndexHnsw> MakeIndexHnsw() {
        return MakeUnique<
            IndexHnsw>(index_name, nullptr, filename, column_names, metric_type, encode_type, build_type, M, ef_construction, block_size, lsg_config);
    }

    UniquePtr<ColumnDef> MakeColumnDef() {
        auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, dim);
        auto data_type = MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo);
        return MakeUnique<ColumnDef>(0, data_type, column_names[0], std::set<ConstraintType>());
    }

protected:
    SizeT M;
    SizeT ef_construction;
    SizeT block_size;
    SizeT dim;
    SizeT element_size;

    SharedPtr<String> index_name;
    String filename;
    Vector<String> column_names;
    MetricType metric_type;
    HnswEncodeType encode_type;
    HnswBuildType build_type;
    Optional<LSGConfig> lsg_config;

    using LabelT = u32;
};

TEST_F(LSGHnswHandlerBuildTest, test_avg) {
    element_size = 128;
    lsg_config->sample_ratio_ = 0.1;

    auto data = MakeUnique<float[]>(dim * element_size);
    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;
    for (SizeT i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto iter = DenseVectorIter<f32, LabelT>(data.get(), dim, element_size);

    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def));

    auto avg = lsg_builder.GetLSAvg<decltype(iter), f32, f32>(std::move(iter), element_size, RowID(0, 0));

    auto avg_gt = MakeUnique<float[]>(element_size);
    {
        SizeT ls_k = std::min(lsg_config->ls_k_, element_size);
        auto distances = MakeUnique<float[]>(element_size);
        for (SizeT i = 0; i < element_size; ++i) {
            const float *v = data.get() + i * dim;
            for (SizeT j = 0; j < element_size; ++j) {
                const float *v2 = data.get() + j * dim;
                float sum = 0;
                for (SizeT k = 0; k < dim; ++k) {
                    float diff = v[k] - v2[k];
                    sum += diff * diff;
                }
                // float distance = std::sqrt(sum);
                // distances[j] = distance;
                distances[j] = sum;
            }
            std::sort(distances.get(), distances.get() + element_size);
            float distance_sum = 0;
            for (SizeT j = 0; j < ls_k; ++j) {
                distance_sum += distances[j];
            }
            avg_gt[i] = distance_sum / ls_k;
        }
    }

    for (SizeT i = 0; i < element_size; ++i) {
        printf("i: %zu, avg: %f, avg_gt: %f\n", i, avg[i], avg_gt[i]);
        ASSERT_GE(avg[i], avg_gt[i]);
    }
}

TEST_F(LSGHnswHandlerBuildTest, test1) {
    dim = 16;
    lsg_config->sample_ratio_ = 0.1;

    auto data = MakeUnique<float[]>(dim * element_size);

    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;
    for (SizeT i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def));

    auto iter = DenseVectorIter<f32, LabelT>(data.get(), dim, element_size);

    UniquePtr<HnswIndexInMem> hnsw_index = lsg_builder.MakeImplIter<decltype(iter), f32, f32>(std::move(iter), element_size, RowID(0, 0), false);

    u32 correct_count = 0;
    i32 topk = 1;
    KnnSearchOption search_option{.ef_ = SizeT(topk) * 10};
    for (SizeT i = 0; i < element_size; ++i) {
        const float *query = data.get() + i * dim;
        HnswHandler *hnsw_handler = hnsw_index.get()->get_ptr();
        auto [result_n, d_ptr, v_ptr] = hnsw_handler->SearchIndex<float, LabelT>(query, topk, search_option);
        Vector<Pair<f32, LabelT>> res(result_n);
        for (SizeT i = 0; i < result_n; ++i) {
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
