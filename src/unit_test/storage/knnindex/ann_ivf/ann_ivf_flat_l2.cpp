//
// Created by yzq on 10/31/23.
//
#include "unit_test/base_test.h"

import infinity_exception;
import stl;
import global_resource_usage;
import third_party;
import logger;
import parser;
import knn_flat_ip;
import infinity;
import ann_ivf_flat;

class AnnIVFFlatL2Test : public BaseTest {};

TEST_F(AnnIVFFlatL2Test, test1) {
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
}
