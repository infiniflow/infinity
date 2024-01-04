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

import knn;

using namespace infinity;

class DistFuncTest2 : public BaseTest {};


float F32IPTest(const float *v1, const float *v2, size_t dim) {
    float res = 0;
    for (size_t i = 0; i < dim; ++i) {
        res += v1[i] * v2[i];
    }
    return -res;
}

TEST_F(DistFuncTest2, test2) {
    using LVQ8Store = LVQStore<float, int8_t, LVQIPCache<float, int8_t>>;
    using Distance = LVQIPDist<float, int8_t>;
    using LVQ8Data = LVQ8Store::StoreType;

    size_t dim = 128;
    size_t vec_n = 10000;

    auto vecs1 = std::make_unique<float[]>(dim * vec_n);
    auto vecs2 = std::make_unique<float[]>(dim * vec_n);

    // generate a random vector of float
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(0, 1);
    for (size_t i = 0; i < vec_n; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            vecs1[i * dim + j] = dist(rng);
            vecs2[i * dim + j] = dist(rng);
        }
    }

    auto lvq_store = LVQ8Store::Make(vec_n, dim, 0);
    Distance distance(lvq_store.dim());
    lvq_store.AddVec(vecs1.get(), vec_n);

    for (size_t i = 0; i < vec_n; ++i) {
        const float *v2 = vecs2.get() + i * dim;

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

        float dist2 = F32IPTest(qv1, qv2, dim);

        float err = std::abs((dist1 - dist2) / dist1);
        EXPECT_LT(err, 1e-5);
        // EXPECT_EQ(dist1, dist2);
        // EXPECT_NEAR(dist1, dist2, 1e-5);
    }
}
