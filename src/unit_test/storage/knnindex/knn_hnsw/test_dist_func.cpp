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
#include <cstdint>
#include <random>

import dist_func_l2;
import hnsw_simd_func;
import lvq_store;
import hnsw_common;

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
        int32_t res = I8IPAVXResidual(v1, v2, dim);
        int32_t res2 = I8IPTest(v1, v2, dim);
        EXPECT_EQ(res, res2);
    }
}

TEST_F(DistFuncTest, test2) {
    using LVQ8Store = LVQStore<float, int8_t, LVQL2Cache<float, int8_t>>;
    using Distance = LVQL2Dist<float, int8_t>;
    using LVQ8Data = LVQ8Store::StoreType;

    size_t dim = 200;
    size_t vec_n = 10000;

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

    auto lvq_store = LVQ8Store::Make(vec_n, dim, 0);
    Distance distance(lvq_store.dim());
    DenseVectorIter iter(vecs1.get(), dim, vec_n);
    lvq_store.AddVec(std::move(iter), vec_n);

    for (size_t i = 0; i < vec_n; ++i) {
        // const float *v1 = vecs1.get() + i * dim;
        const float *v2 = vecs2.get() + i * dim;

        // float dist_true = F32L2Test(v1, v2, dim);

        auto query = lvq_store.MakeQuery(v2);
        LVQ8Data lvq1 = query;
        LVQ8Data lvq2 = lvq_store.GetVec(i);

        float dist1 = distance(lvq1, lvq2, lvq_store);

        float qv1[dim];
        float qv2[dim];
        {
            auto c1 = lvq1.GetCompressVec();
            auto c2 = lvq2.GetCompressVec();
            auto mean = lvq_store.GetMean();
            auto [scale1, bias1] = lvq1.GetScalar();
            auto [scale2, bias2] = lvq2.GetScalar();
            for (size_t i = 0; i < dim; ++i) {
                qv1[i] = scale1 * c1[i] + bias1 + mean[i];
                qv2[i] = scale2 * c2[i] + bias2 + mean[i];
            }
        }

        float dist2 = F32L2Test(qv1, qv2, dim);

        // std::cout << dist1 << "\t" << dist2 << "\t" << dist_true << std::endl;
        float err = std::abs((dist1 - dist2) / dist1);
        EXPECT_LT(err, 1e-5);
        // EXPECT_EQ(dist1, dist2);
        // EXPECT_NEAR(dist1, dist2, 1e-5);
    }
}
