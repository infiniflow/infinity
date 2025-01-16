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
import hnsw_lsg_builder;
import index_hnsw;
import column_def;
import embedding_info;
import data_type;
import hnsw_common;
import logical_type;
import index_base;
import internal_types;
import abstract_hnsw;
import infinity_exception;
import hnsw_alg;

using namespace infinity;

class LSGBuildTest : public BaseTest {
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
    }

    UniquePtr<IndexHnsw> MakeIndexHnsw() {
        return MakeUnique<
            IndexHnsw>(index_name, nullptr, filename, column_names, metric_type, encode_type, build_type, M, ef_construction, block_size);
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

    using LabelT = u32;
};

TEST_F(LSGBuildTest, test_avg) {
    element_size = 128;

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

    LSGConfig lsg_config;
    lsg_config.sample_raito_ = 0.1;
    HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def), std::move(lsg_config));

    auto avg = lsg_builder.GetLSAvg<decltype(iter), f32, f32>(std::move(iter), element_size, RowID(0, 0));

    auto avg_gt = MakeUnique<float[]>(element_size);
    {
        SizeT sample_num = element_size * lsg_config.sample_raito_;
        Vector<SizeT> sample_idx(sample_num);
        for (SizeT i = 0; i < sample_num; ++i) {
            sample_idx[i] = rand() % element_size;
        }
        SizeT ls_k = std::min(lsg_config.ls_k_, sample_num);
        auto distances = MakeUnique<float[]>(sample_num);
        for (SizeT i = 0; i < element_size; ++i) {
            const float *v = data.get() + i * dim;
            for (SizeT j = 0; j < sample_num; ++j) {
                SizeT idx = sample_idx[j];
                const float *v2 = data.get() + idx * dim;
                float sum = 0;
                for (SizeT k = 0; k < dim; ++k) {
                    float diff = v[k] - v2[k];
                    sum += diff * diff;
                }
                // float distance = std::sqrt(sum);
                // distances[j] = distance;
                distances[j] = sum;
            }
            std::sort(distances.get(), distances.get() + sample_num);
            float distance_sum = 0;
            for (SizeT j = 0; j < ls_k; ++j) {
                distance_sum += distances[j];
            }
            avg_gt[i] = distance_sum / ls_k;
        }
    }

    for (SizeT i = 0; i < element_size; ++i) {
        printf("avg: %f, avg_gt: %f\n", avg[i], avg_gt[i]);
        float diff_rate = std::abs(avg[i] - avg_gt[i]) / avg_gt[i];
        ASSERT_LE(diff_rate, 0.5);
    }
}

TEST_F(LSGBuildTest, test1) {
    dim = 16;

    auto data = MakeUnique<float[]>(dim * element_size);

    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;
    for (SizeT i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto index_hnsw = MakeIndexHnsw();
    auto column_def = MakeColumnDef();

    LSGConfig lsg_config;
    lsg_config.sample_raito_ = 0.1;

    HnswLSGBuilder lsg_builder(index_hnsw.get(), std::move(column_def), std::move(lsg_config));

    auto iter = DenseVectorIter<f32, LabelT>(data.get(), dim, element_size);

    UniquePtr<HnswIndexInMem> hnsw_index = lsg_builder.MakeImplIter<decltype(iter), f32, f32>(std::move(iter), element_size, RowID(0, 0), false);

    auto search_index = [](HnswIndexInMem *hnsw_index, const float *query, i32 topk, const KnnSearchOption &search_option) -> Vector<Pair<f32, LabelT>> {
        return std::visit(
            [&](auto &&index) {
                using T = std::decay_t<decltype(index)>;
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    UnrecoverableError("Invalid index type");
                } else {
                    using IndexT = std::decay_t<decltype(*index)>;
                    using DataType = typename IndexT::DataType;
                    if constexpr (std::is_same_v<DataType, f32>) {
                        return index->KnnSearchSorted(query, topk, search_option);
                    }
                    UnrecoverableError("Invalid data type");
                }
                return Vector<Pair<f32, LabelT>>{};
            },
            hnsw_index->get());
    };

    u32 correct_count = 0;
    i32 topk = 1;
    KnnSearchOption search_option{.ef_ = SizeT(topk) * 10};
    for (SizeT i = 0; i < element_size; ++i) {
        const float *query = data.get() + i * dim;
        Vector<Pair<f32, LabelT>> res = search_index(hnsw_index.get(), query, topk, search_option);
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
