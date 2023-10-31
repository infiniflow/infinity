//
// Created by yzq on 10/31/23.
//
#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import parser;
import knn_flat_ip;
import infinity;
import ann_ivf_flat;
import knn_flat_l2;
#include <random>

using namespace infinity;

i32 match_test(UniquePtr<f32[]> &base_embedding,
               UniquePtr<f32[]> &query_embedding,
               i32 dimension,
               i32 top_k,
               i32 base_embedding_count,
               i32 n_lists = 100,
               i32 n_probes = 5);

class AnnIVFFlatL2RandomTest : public BaseTest {};

/*
TEST_F(AnnIVFFlatL2RandomTest, test1) {
    i32 dimension = 4;
    i32 base_embedding_count = 10000;
    i32 top_k = 30;
    i32 n_lists = 100;
    i32 n_probes = 5;
    UniquePtr<f32[]> base_embedding = MakeUnique<f32[]>(sizeof(f32) * dimension * base_embedding_count);
    UniquePtr<f32[]> query_embedding = MakeUnique<f32[]>(sizeof(f32) * dimension);

    std::cout << "#################################################" << std::endl;
    std::cout << "dimension: " << dimension << std::endl;
    std::cout << "top_k: " << top_k << std::endl;
    std::cout << "base_embedding_count: " << base_embedding_count << std::endl;
    std::cout << "#################################################" << std::endl;

    // match_test 1000 times,check result
    for (int i = 0; i < 100; ++i) {
        auto matches = match_test(base_embedding, query_embedding, dimension, top_k, base_embedding_count);
        // show correct rate
        std::cout << "match rate: " << 100 * matches / (f32)top_k << "%" << std::endl;
    }

    std::cout << "#################################################" << std::endl;
    std::cout << "dimension: " << dimension << std::endl;
    std::cout << "top_k: " << top_k << std::endl;
    std::cout << "base_embedding_count: " << base_embedding_count << std::endl;
    std::cout << "#################################################" << std::endl;
}
*/
i32 match_test(UniquePtr<f32[]> &base_embedding,
               UniquePtr<f32[]> &query_embedding,
               i32 dimension,
               i32 top_k,
               i32 base_embedding_count,
               i32 n_lists,
               i32 n_probes) {

    // use std random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<f32> dis(-100.0, 100.0);
    for (i32 i = 0; i < base_embedding_count; ++i) {
        for (i32 j = 0; j < dimension; ++j) {
            base_embedding[i * dimension + j] = dis(gen);
        }
    }
    for (i32 i = 0; i < dimension; ++i) {
        query_embedding[i] = dis(gen);
    }

    AnnIVFFlatL2<f32> ann_distance(query_embedding.get(), 1, top_k, dimension, EmbeddingDataType::kElemFloat);

    auto ann_ivf_l2_index = AnnIVFFlatL2<f32>::CreateIndex(dimension, base_embedding_count, base_embedding.get(), n_lists, 0, 0);

    ann_distance.Begin();
    ann_distance.Search(ann_ivf_l2_index.get(), n_probes);
    ann_distance.End();

    f32 *ann_distance_array = ann_distance.GetDistanceByIdx(0);
    RowID *ann_id_array = ann_distance.GetIDByIdx(0);

    KnnFlatL2<f32> knn_distance(query_embedding.get(), 1, top_k, dimension, EmbeddingDataType::kElemFloat);

    knn_distance.Begin();
    knn_distance.Search(base_embedding.get(), base_embedding_count, 0, 0);
    knn_distance.End();

    f32 *knn_distance_array = knn_distance.GetDistanceByIdx(0);
    RowID *knn_id_array = knn_distance.GetIDByIdx(0);

    i32 match_count = 0;
    for (i32 i = 0; i < top_k; ++i) {
        // print the result id
        // std::cout << "ann_id: " << ann_id_array[i].block_offset_ << " knn_id: " << knn_id_array[i].block_offset_ << std::endl;
        // std::cout << "ann_distance: " << ann_distance_array[i] << " knn_distance: " << knn_distance_array[i] << std::endl;

        if (ann_id_array[i] == knn_id_array[i]) {
            match_count++;
        }
        if (ann_id_array[i] != knn_id_array[i]) {
            //    std::cout << "!!!!!!!!!!!!!!! id different!" << std::endl;
        }
    }
    return match_count;
}
