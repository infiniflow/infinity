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

#include "../../../../storage/knn_index/header.h"
#include "unit_test/base_test.h"
#include <random>

import bmp_simd_func;
import stl;

using namespace infinity;

class BMPSIMDTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(BMPSIMDTest, test1) {
    std::default_random_engine rng;
    std::uniform_real_distribution<float> rdist(0, 1);

    auto test_func = [&](SizeT dim, SizeT max_dim) {
        Vector<f32> data(dim);
        Vector<f32> dest(max_dim);
        f32 x = rdist(rng);

        std::uniform_int_distribution<int> idist(0, max_dim - 1);
        HashSet<i32> idx_set;
        while (idx_set.size() < dim) {
            i32 i = idist(rng);
            idx_set.insert(i);
        }
        Vector<i32> idx(idx_set.begin(), idx_set.end());
        std::sort(idx.begin(), idx.end());

        for (SizeT i = 0; i < dim; ++i) {
            data[i] = rdist(rng);
        }
        for (SizeT i = 0; i < max_dim; ++i) {
            dest[i] = rdist(rng);
        }

        Vector<f32> groundtruth(dest);
        for (SizeT i = 0; i < dim; ++i) {
            groundtruth[idx[i]] += data[i] * x;
        }
        MultiF32StoreI32(idx.data(), data.data(), dest.data(), x, dim);

        for (SizeT i : idx) {
            ASSERT_FLOAT_EQ(dest[i], groundtruth[i]);
        }
    };
    for (SizeT i = 1; i <= 25; ++i) {
        test_func(i, 100);
    }
}
