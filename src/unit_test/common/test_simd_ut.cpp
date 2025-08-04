// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.test_simd;

import :ut.base_test;
import :stl;
import :simd_init;
#endif

using namespace infinity;

class SimdInitTest : public BaseTest {};

TEST_F(SimdInitTest, GetSupportedSimdTypesList) {
    std::cout << "Supported SIMD types:";
    for (const auto &simd_type : infinity::GetSupportedSimdTypesList()) {
        std::cout << " " << simd_type;
    }
    std::cout << std::endl;
    // check endianess for src\common\simd\maxsim_simd_funcs.cppm:66
    alignas(alignof(u16)) u8 v[2] = {1, 0};
    EXPECT_EQ(*reinterpret_cast<const u16 *>(v), 1u);
}
