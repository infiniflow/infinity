//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <iostream>

import stl;
import bitmask;
import plain_store;
import lvq_store;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;

using namespace infinity;

#define EXPECT_VALUE_EQ(a, b)                                                                                                                        \
    if (auto f = f64(a) - f64(b); Max(f, -f) > 1e-4) {                                                                                               \
        std::cerr << "values aren't equal at line\t" << __LINE__ << "\tvalues: " << a << " != " << b << std::endl;                                   \
    }

int main() {
    i64 dimension = 4;
    i64 top_k = 4;
    i64 base_embedding_count = 4;
    UniquePtr<f32[]> base_embedding = MakeUnique<f32[]>(sizeof(f32) * dimension * base_embedding_count);
    UniquePtr<f32[]> query_embedding = MakeUnique<f32[]>(sizeof(f32) * dimension);

    {
        base_embedding[0] = 0.1;
        base_embedding[1] = 0.2;
        base_embedding[2] = 0.3;
        base_embedding[3] = 0.4;
    }

    {
        base_embedding[4] = 0.2;
        base_embedding[5] = 0.1;
        base_embedding[6] = 0.3;
        base_embedding[7] = 0.4;
    }

    {
        base_embedding[8] = 0.3;
        base_embedding[9] = 0.2;
        base_embedding[10] = 0.1;
        base_embedding[11] = 0.4;
    }

    {
        base_embedding[12] = 0.4;
        base_embedding[13] = 0.3;
        base_embedding[14] = 0.2;
        base_embedding[15] = 0.1;
    }

    {
        query_embedding[0] = 0.1;
        query_embedding[1] = 0.2;
        query_embedding[2] = 0.3;
        query_embedding[3] = 0.4;
    }

    using LabelT = u64;
    using Hnsw = KnnHnsw<f32, LabelT, LVQStore<f32, i8, LVQL2Cache<f32, i8>>, LVQL2Dist<f32, i8>>;
    int M = 16;
    int ef_construction = 200;
    auto hnsw_index = Hnsw::Make(base_embedding_count, dimension, M, ef_construction, {});
    auto labels = MakeUnique<LabelT[]>(base_embedding_count);
    for (int i = 0; i < base_embedding_count; ++i) {
        labels[i] = i;
    }
    hnsw_index->InsertVecs(base_embedding.get(), labels.get(), base_embedding_count);

    Vector<f32> distance_array(top_k);
    Vector<u64> id_array(top_k);
    {
        auto result = hnsw_index->KnnSearch(query_embedding.get(), top_k);

        EXPECT_VALUE_EQ(result[0].first, 0);
        EXPECT_VALUE_EQ(result[0].second, 0);

        EXPECT_VALUE_EQ(result[1].first, 0.02);
        EXPECT_VALUE_EQ(result[1].second, 1);

        EXPECT_VALUE_EQ(result[2].first, 0.08);
        EXPECT_VALUE_EQ(result[2].second, 2);

        EXPECT_VALUE_EQ(result[3].first, 0.2);
        EXPECT_VALUE_EQ(result[3].second, 3);
    }

    auto p_bitmask = Bitmask::Make(64);
    p_bitmask->SetFalse(1);
    --top_k;
    {
        auto result = hnsw_index->KnnSearch(query_embedding.get(), top_k, *p_bitmask);

        EXPECT_VALUE_EQ(result[0].first, 0);
        EXPECT_VALUE_EQ(result[0].second, 0);

        EXPECT_VALUE_EQ(result[1].first, 0.08);
        EXPECT_VALUE_EQ(result[1].second, 2);

        EXPECT_VALUE_EQ(result[2].first, 0.2);
        EXPECT_VALUE_EQ(result[2].second, 3);
    }

    p_bitmask->SetFalse(0);
    --top_k;
    {
        auto result = hnsw_index->KnnSearch(query_embedding.get(), top_k, *p_bitmask);

        EXPECT_VALUE_EQ(result[0].first, 0.08);
        EXPECT_VALUE_EQ(result[0].second, 2);

        EXPECT_VALUE_EQ(result[1].first, 0.2);
        EXPECT_VALUE_EQ(result[1].second, 3);
    }

    p_bitmask->SetFalse(2);
    --top_k;
    {
        auto result = hnsw_index->KnnSearch(query_embedding.get(), top_k, *p_bitmask);

        EXPECT_VALUE_EQ(result[0].first, 0.2);
        EXPECT_VALUE_EQ(result[0].second, 3);
    }
}
