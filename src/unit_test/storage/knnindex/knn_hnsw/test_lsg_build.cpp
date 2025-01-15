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

using namespace infinity;

class LSGBuildTest : public BaseTest {
public:
};

TEST_F(LSGBuildTest, test1) {
    SizeT dim = 128;
    int element_size = 1000;
    auto data = MakeUnique<float[]>(dim * element_size);

    std::mt19937 rng;
    rng.seed(0);
    std::uniform_real_distribution<float> distrib_real;
    for (SizeT i = 0; i < dim * element_size; ++i) {
        data[i] = distrib_real(rng);
    }

    auto index_name = MakeShared<String>("index_name");
    String filename = "filename";
    Vector<String> column_names = {"col_name"};
    MetricType metric_type = MetricType::kMetricL2;
    HnswEncodeType encode_type = HnswEncodeType::kPlain;
    SizeT M = 8;
    SizeT ef_construction = 200;
    SizeT block_size = 8192;
    auto index_hnsw = MakeUnique<IndexHnsw>(index_name, nullptr, filename, column_names, metric_type, encode_type, M, ef_construction, block_size);

    auto embeddingInfo = MakeShared<EmbeddingInfo>(EmbeddingDataType::kElemFloat, dim);
    auto data_type = MakeShared<DataType>(LogicalType::kEmbedding, embeddingInfo);
    auto column_def = MakeShared<ColumnDef>(0, data_type, column_names[0], std::set<ConstraintType>());

    LSGConfig lsg_config;
    lsg_config.sample_raito_ = 0.1;

    [[maybe_unused]] HnswLSGBuilder lsg_builder(index_hnsw.get(), column_def, std::move(lsg_config));

    auto iter = DenseVectorIter<f32, i32>(data.get(), dim, element_size);
    UniquePtr<HnswIndexInMem> hnsw_index = lsg_builder.Make(std::move(iter), element_size, RowID(0, 0));
}
