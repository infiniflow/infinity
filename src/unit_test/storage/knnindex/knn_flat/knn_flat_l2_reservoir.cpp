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
import base_test;

import infinity_exception;

import stl;
import global_resource_usage;
import third_party;
import logger;

import deprecated_knn_flat_l2_reservoir;
import infinity_context;
import knn_expr;
import internal_types;

using namespace infinity;

class KnnFlatL2ReservoirTest : public BaseTest {};

TEST_F(KnnFlatL2ReservoirTest, test1) {
    using namespace infinity;

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

    KnnFlatL2Reservoir<f32> knn_distance(query_embedding.get(), 1, top_k, dimension, EmbeddingDataType::kElemFloat);

    knn_distance.Begin();
    knn_distance.Search(base_embedding.get(), base_embedding_count, 0, 0);
    knn_distance.End();

    f32 *distance_array = knn_distance.GetDistanceByIdx(0);
    RowID *id_array = knn_distance.GetIDByIdx(0);
    EXPECT_FLOAT_EQ(distance_array[0], 0);
    EXPECT_FLOAT_EQ(id_array[0].segment_id_, 0);
    EXPECT_FLOAT_EQ(id_array[0].segment_offset_, 0);

    EXPECT_FLOAT_EQ(distance_array[1], 0.02);
    EXPECT_FLOAT_EQ(id_array[1].segment_id_, 0);
    EXPECT_FLOAT_EQ(id_array[1].segment_offset_, 1);

    EXPECT_FLOAT_EQ(distance_array[2], 0.08);
    EXPECT_FLOAT_EQ(id_array[2].segment_id_, 0);
    EXPECT_FLOAT_EQ(id_array[2].segment_offset_, 2);

    EXPECT_FLOAT_EQ(distance_array[3], 0.2);
    EXPECT_FLOAT_EQ(id_array[3].segment_id_, 0);
    EXPECT_FLOAT_EQ(id_array[3].segment_offset_, 3);
}
