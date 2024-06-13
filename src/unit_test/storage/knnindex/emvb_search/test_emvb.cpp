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

#include "unit_test/base_test.h"

import infinity_exception;
import stl;
import emvb_search;
import emvb_product_quantization;
import emvb_shared_vec;

namespace infinity {
extern template class EMVBSharedVec<u32>;
extern template class EMVBSearch<32>;
extern template class EMVBSearch<64>;
extern template class EMVBSearch<96>;
extern template class EMVBSearch<128>;
extern template class EMVBSearch<160>;
extern template class EMVBSearch<192>;
extern template class EMVBSearch<224>;
extern template class EMVBSearch<256>;
}

using namespace infinity;

class FakePQ final : public EMVBProductQuantizer {
public:
    void Train(const f32 *embedding_data, u32 embedding_num, u32 iter_cnt) override {}
    void AddEmbeddings(const f32 *embedding_data, u32 embedding_num) override {}
    UniquePtr<f32[]> GetIPDistanceTable(const f32 *query_data, u32 query_num) const override { return nullptr; }
    f32 GetSingleIPDistance(u32 embedding_id, u32 query_id, u32 query_num, const f32 *ip_table) const override { return 0.0f; }
    void
    GetMultipleIPDistance(u32 embedding_offset, u32 embedding_num, u32 query_id, u32 query_num, const f32 *ip_table, f32 *output_ptr) const override {
        std::fill_n(output_ptr, embedding_num, 0.0f);
    }
};

class EMVBTest : public BaseTest {};

TEST_F(EMVBTest, test_fakepq) {
    constexpr u32 embedding_dimension = 128;
    constexpr u32 centroid_num = 8;
    constexpr u32 docs_in_one_centroid = 10;
    constexpr u32 n_docs = centroid_num * docs_in_one_centroid;
    constexpr u32 embedding_in_one_doc = 10;
    constexpr u32 FIXED_QUERY_TOKEN_NUM = 32;

    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_real_distribution<f32> dis(-1024.0f, 1024.0f);
    Vector<u32> doc_lens(n_docs, embedding_in_one_doc);
    Vector<u32> doc_offsets(n_docs);
    for (u32 i = 1; i < n_docs; ++i) {
        doc_offsets[i] = doc_offsets[i - 1] + doc_lens[i - 1];
    }
    Vector<u32> centroid_id_assignments(n_docs * embedding_in_one_doc);
    for (u32 i = 0; i < n_docs; ++i) {
        for (u32 j = 0; j < embedding_in_one_doc; ++j) {
            centroid_id_assignments[i * embedding_in_one_doc + j] = i / docs_in_one_centroid;
        }
    }
    Vector<f32> centroids_data(centroid_num * embedding_dimension);
    for (u32 i = 0; i < centroid_num; ++i) {
        centroids_data[i * embedding_dimension + i] = 1.0f;
    }
    const auto centroids_to_docid = MakeUnique<EMVBSharedVec<u32>[]>(centroid_num);
    for (u32 i = 0; i < centroid_num; ++i) {
        for (u32 j = 0; j < docs_in_one_centroid; ++j) {
            centroids_to_docid[i].PushBack(i * docs_in_one_centroid + j);
        }
    }
    FakePQ fake_pq;
    auto emvb = EMVBSearch<FIXED_QUERY_TOKEN_NUM>(embedding_dimension,
                                                  n_docs,
                                                  centroid_num,
                                                  doc_lens.data(),
                                                  doc_offsets.data(),
                                                  centroid_id_assignments.data(),
                                                  centroids_data.data(),
                                                  centroids_to_docid.get(),
                                                  &fake_pq);
    Vector<f32> query(FIXED_QUERY_TOKEN_NUM * embedding_dimension);
    for (u32 i = 0; i < FIXED_QUERY_TOKEN_NUM; ++i) {
        query[i * embedding_dimension + 3] = 1.0f;
    }
    u32 nprobe = 1;
    f32 thresh = 0.5;
    u32 n_doc_to_score = 100;
    u32 out_second_stage = 50;
    u32 k = 10;
    f32 thresh_query = 0.8;
    auto [n_result, scores, doc_ids] = emvb.GetQueryResult(query.data(), nprobe, thresh, n_doc_to_score, out_second_stage, k, thresh_query);
    EXPECT_EQ(n_result, k);
    std::sort(doc_ids.get(), doc_ids.get() + n_result);
    for (u32 i = 0; i < n_result; ++i) {
        EXPECT_FLOAT_EQ(scores[i], 32.0f);
        EXPECT_EQ(doc_ids[i], 30 + i);
    }
}
