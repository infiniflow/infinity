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

#include "../../../../storage/knn_index/knn_hnsw/header.h"
#include "unit_test/base_test.h"
#include <cstdint>
#include <random>

import dist_func_l2;
import data_store;
import vec_store_type;
import hnsw_simd_func;
import hnsw_common;
import stl;

using namespace infinity;

class DistFuncTest : public BaseTest {};

int32_t I8IPTest(const int8_t *v1, const int8_t *v2, size_t dim) {
    int32_t res = 0;
    for (size_t i = 0; i < dim; ++i) {
        res += int16_t(v1[i]) * v2[i];
    }
    return res;
}

float F32L2Test(const float *v1, const float *v2, size_t dim) {
    float res = 0;
    for (size_t i = 0; i < dim; ++i) {
        float t = v1[i] - v2[i];
        res += t * t;
    }
    return res;
}

TEST_F(DistFuncTest, test1) {
    size_t dim = 200;
    size_t vec_n = 10000;

    auto vecs1 = std::make_unique<int8_t[]>(dim * vec_n);
    auto vecs2 = std::make_unique<int8_t[]>(dim * vec_n);

    // generate a random vector of int8_t
    std::default_random_engine rng;
    std::uniform_int_distribution<int8_t> dist(-128, 127);
    for (size_t i = 0; i < vec_n; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            vecs1[i * dim + j] = dist(rng);
            vecs2[i * dim + j] = dist(rng);
        }
    }

    for (size_t i = 0; i < vec_n; ++i) {
        auto v1 = vecs1.get() + i * dim;
        auto v2 = vecs2.get() + i * dim;

        int32_t res = I8IPSSEResidual(v1, v2, dim);
        int32_t res2 = I8IPTest(v1, v2, dim);
        EXPECT_EQ(res, res2);
    }
}

TEST_F(DistFuncTest, test2) {
    using LabelT = int;
    using VecStoreType = LVQL2VecStoreType<float, int8_t>;
    using DataStore = DataStore<VecStoreType, LabelT>;
    using Distance = typename VecStoreType::Distance;
    using LVQ8Data = typename VecStoreType::StoreType;

    size_t dim = 200;
    size_t vec_n = 8192;
    size_t max_chunk_size = 1;

    auto vecs1 = std::make_unique<float[]>(dim * vec_n);
    auto vecs2 = std::make_unique<float[]>(dim * vec_n);

    // generate a random vector of float
    std::default_random_engine rng;
    std::uniform_real_distribution<float> rdist(0, 1);
    for (size_t i = 0; i < vec_n; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            vecs1[i * dim + j] = rdist(rng);
            vecs2[i * dim + j] = rdist(rng);
        }
    }

    auto lvq_store = DataStore::Make(vec_n, max_chunk_size, dim, 0 /*Mmax0*/, 0 /*Mmax*/);
    Distance distance(lvq_store.dim());
    DenseVectorIter iter(vecs1.get(), dim, vec_n, 0);
    auto [start_i, end_i] = lvq_store.AddVec(std::move(iter));
    EXPECT_EQ(start_i, 0u);
    EXPECT_EQ(end_i, vec_n);

    for (size_t i = 0; i < vec_n; ++i) {
        // const float *v1 = vecs1.get() + i * dim;
        const float *v2 = vecs2.get() + i * dim;

        // float dist_true = F32L2Test(v1, v2, dim);

        auto query = lvq_store.MakeQuery(v2);
        LVQ8Data lvq1 = query;
        LVQ8Data lvq2 = lvq_store.GetVec(i);

        const auto &vec_store_meta = lvq_store.vec_store_meta();
        float dist1 = distance(lvq1, lvq2, vec_store_meta);

        Vector<float> qv1(dim);
        Vector<float> qv2(dim);
        {
            const auto *c1 = lvq1->compress_vec_;
            const auto *c2 = lvq2->compress_vec_;
            const auto *mean = vec_store_meta.mean();
            auto scale1 = lvq1->scale_;
            auto bias1 = lvq1->bias_;
            auto scale2 = lvq2->scale_;
            auto bias2 = lvq2->bias_;
            for (size_t i = 0; i < dim; ++i) {
                qv1[i] = scale1 * c1[i] + bias1 + mean[i];
                qv2[i] = scale2 * c2[i] + bias2 + mean[i];
            }
        }

        float dist2 = F32L2Test(qv1.data(), qv2.data(), dim);

        // std::cout << dist1 << "\t" << dist2 << "\t" << dist_true << std::endl;
        float err = std::abs((dist1 - dist2) / dist1);
        EXPECT_LT(err, 1e-5);
        // EXPECT_EQ(dist1, dist2);
        // EXPECT_NEAR(dist1, dist2, 1e-5);
    }
}
