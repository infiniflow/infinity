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

#include "unit_test/base_test.h"

import stl;
import bitmask;
import data_store;
import vec_store_type;
import dist_func_l2;
import dist_func_ip;
import hnsw_alg;
import knn_filter;
import hnsw_common;

using namespace infinity;

class HnswAlgBitmaskTest : public BaseTest {
public:
    static constexpr float error = 1e-4;
};

TEST_F(HnswAlgBitmaskTest, test1) {
    i64 dimension = 4;
    i64 top_k = 4;
    i64 base_embedding_count = 4;
    int max_chunk_n = 1;
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
    using Hnsw = KnnHnsw<LVQL2VecStoreType<f32, i8>, LabelT>;
    int M = 16;
    int ef_construction = 200;
    auto hnsw_index = Hnsw::Make(base_embedding_count, max_chunk_n, dimension, M, ef_construction);

    auto iter = DenseVectorIter<f32, LabelT>(base_embedding.get(), dimension, base_embedding_count);
    hnsw_index->InsertVecs(std::move(iter));

    Vector<f32> distance_array(top_k);
    Vector<u64> id_array(top_k);
    KnnSearchOption search_option{.ef_ = 2ul * top_k};
    {
        auto result = hnsw_index->KnnSearchSorted(query_embedding.get(), top_k, search_option);

        EXPECT_NEAR(result[0].first, 0, error);
        EXPECT_NEAR(result[0].second, 0, error);

        EXPECT_NEAR(result[1].first, 0.02, error);
        EXPECT_NEAR(result[1].second, 1, error);

        EXPECT_NEAR(result[2].first, 0.08, error);
        EXPECT_NEAR(result[2].second, 2, error);

        EXPECT_NEAR(result[3].first, 0.2, error);
        EXPECT_NEAR(result[3].second, 3, error);
    }

    auto p_bitmask = Bitmask::Make(64);
    p_bitmask->SetFalse(1);
    --top_k;
    {
        BitmaskFilter<LabelT> filter(*p_bitmask);
        auto result = hnsw_index->KnnSearchSorted(query_embedding.get(), top_k, filter, search_option);

        EXPECT_NEAR(result[0].first, 0, error);
        EXPECT_NEAR(result[0].second, 0, error);

        EXPECT_NEAR(result[1].first, 0.08, error);
        EXPECT_NEAR(result[1].second, 2, error);

        EXPECT_NEAR(result[2].first, 0.2, error);
        EXPECT_NEAR(result[2].second, 3, error);
    }

    p_bitmask->SetFalse(0);
    --top_k;
    {
        BitmaskFilter<LabelT> filter(*p_bitmask);
        auto result = hnsw_index->KnnSearchSorted(query_embedding.get(), top_k, filter, search_option);

        EXPECT_NEAR(result[0].first, 0.08, error);
        EXPECT_NEAR(result[0].second, 2, error);

        EXPECT_NEAR(result[1].first, 0.2, error);
        EXPECT_NEAR(result[1].second, 3, error);
    }

    p_bitmask->SetFalse(2);
    --top_k;
    {
        BitmaskFilter<LabelT> filter(*p_bitmask);
        auto result = hnsw_index->KnnSearchSorted(query_embedding.get(), top_k, filter, search_option);

        EXPECT_NEAR(result[0].first, 0.2, error);
        EXPECT_NEAR(result[0].second, 3, error);
    }
}