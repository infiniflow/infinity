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
import knn_filter;
import ann_ivf_flat;
import bitmask;
import knn_expr;
import internal_types;
import infinity_context;
import global_resource_usage;

using namespace infinity;

class AnnIVFFlatL2Test : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         AnnIVFFlatL2Test,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(AnnIVFFlatL2Test, test1) {
    using namespace infinity;

    i64 dimension = 4;
    i64 top_k = 4;
    i64 base_embedding_count = 4;
    UniquePtr<f32[]> base_embedding = MakeUnique<f32[]>(dimension * base_embedding_count);
    UniquePtr<f32[]> query_embedding = MakeUnique<f32[]>(dimension);

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

    AnnIVFFlatL2<f32> ann_distance(query_embedding.get(), 1, top_k, dimension, EmbeddingDataType::kElemFloat);

    auto ann_ivf_l2_index = AnnIVFFlatL2<f32>::CreateIndex(dimension, base_embedding_count, base_embedding.get(), 1);

    ann_distance.Begin();
    ann_distance.Search(ann_ivf_l2_index.get(), 0, 1);
    ann_distance.End();

    f32 *distance_array = ann_distance.GetDistanceByIdx(0);
    RowID *id_array = ann_distance.GetIDByIdx(0);
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

    {
        AnnIVFFlatL2<f32> ann_distance_m(query_embedding.get(), 1, top_k, dimension, EmbeddingDataType::kElemFloat);
        auto p_bitmask = Bitmask::Make(64);
        BitmaskFilter<SegmentOffset> filter(*p_bitmask);
        p_bitmask->SetFalse(1);
        {
            ann_distance_m.Begin();
            ann_distance_m.Search(ann_ivf_l2_index.get(), 0, 1, filter);
            ann_distance_m.End();
            f32 *distance_array_m = ann_distance_m.GetDistanceByIdx(0);
            RowID *id_array_m = ann_distance_m.GetIDByIdx(0);
            EXPECT_FLOAT_EQ(distance_array_m[0], 0);
            EXPECT_FLOAT_EQ(id_array_m[0].segment_id_, 0);
            EXPECT_FLOAT_EQ(id_array_m[0].segment_offset_, 0);

            EXPECT_FLOAT_EQ(distance_array_m[1], 0.08);
            EXPECT_FLOAT_EQ(id_array_m[1].segment_id_, 0);
            EXPECT_FLOAT_EQ(id_array_m[1].segment_offset_, 2);

            EXPECT_FLOAT_EQ(distance_array_m[2], 0.2);
            EXPECT_FLOAT_EQ(id_array_m[2].segment_id_, 0);
            EXPECT_FLOAT_EQ(id_array_m[2].segment_offset_, 3);
        }

        p_bitmask->SetFalse(0);
        {
            ann_distance_m.ReInitialize();
            ann_distance_m.Search(ann_ivf_l2_index.get(), 0, 1, filter);
            ann_distance_m.End();
            f32 *distance_array_m = ann_distance_m.GetDistanceByIdx(0);
            RowID *id_array_m = ann_distance_m.GetIDByIdx(0);

            EXPECT_FLOAT_EQ(distance_array_m[0], 0.08);
            EXPECT_FLOAT_EQ(id_array_m[0].segment_id_, 0);
            EXPECT_FLOAT_EQ(id_array_m[0].segment_offset_, 2);

            EXPECT_FLOAT_EQ(distance_array_m[1], 0.2);
            EXPECT_FLOAT_EQ(id_array_m[1].segment_id_, 0);
            EXPECT_FLOAT_EQ(id_array_m[1].segment_offset_, 3);
        }

        p_bitmask->SetFalse(2);
        {
            ann_distance_m.ReInitialize();
            ann_distance_m.Search(ann_ivf_l2_index.get(), 0, 1, filter);
            ann_distance_m.End();
            f32 *distance_array_m = ann_distance_m.GetDistanceByIdx(0);
            RowID *id_array_m = ann_distance_m.GetIDByIdx(0);

            EXPECT_FLOAT_EQ(distance_array_m[0], 0.2);
            EXPECT_FLOAT_EQ(id_array_m[0].segment_id_, 0);
            EXPECT_FLOAT_EQ(id_array_m[0].segment_offset_, 3);
        }
    }
}
