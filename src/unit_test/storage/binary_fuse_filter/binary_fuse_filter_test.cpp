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

import stl;
import binary_fuse_filter;

class BinaryFuseFilterTest : public BaseTest {};

TEST_F(BinaryFuseFilterTest, test_2000) {
    using namespace infinity;
    constexpr u64 NUM = 2000;
    Array<u64, NUM> data;
    for (u64 i = 0; i < NUM; ++i) {
        data[i] = i * NUM;
    }
    BinaryFuse filter;
    // test build
    EXPECT_NO_THROW(filter.Build(0, data.data(), NUM));
    // test contain
    for (u64 i = 0; i < NUM; ++i) {
        u64 item = i * NUM;
        EXPECT_EQ(filter.Contain(1, item), true);
    }
    // test fake contain
    u64 fake_contain = 0;
    u64 total_cnt = 0;
    for (u64 i = 0; i < NUM * NUM; ++i) {
        if (i % NUM) {
            ++total_cnt;
            if (filter.Contain(1, i)) {
                ++fake_contain;
            }
        }
    }
    f64 ratio = static_cast<f64>(fake_contain) / total_cnt;
    EXPECT_LT(ratio, 0.005);
}
