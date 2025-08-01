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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.global_resource_usage;

import :ut.base_test;
import :stl;
#endif

import global_resource_usage;

using namespace infinity;
class GlobalResourceUsageTest : public BaseTest {
    void SetUp() override {
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
    }

    void TearDown() override {
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
    }
};

TEST_F(GlobalResourceUsageTest, usage_test) {
    using namespace infinity;

    // Object count
#ifdef INFINITY_STATS
    GlobalResourceUsage::IncrObjectCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetObjectCount(), 1);

    GlobalResourceUsage::IncrObjectCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetObjectCount(), 2);

    GlobalResourceUsage::DecrObjectCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetObjectCount(), 1);

    GlobalResourceUsage::DecrObjectCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetObjectCount(), 0);

    GlobalResourceUsage::DecrObjectCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetObjectCount(), -1);

    GlobalResourceUsage::IncrObjectCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetObjectCount(), 0);

    // Raw memory count
    GlobalResourceUsage::IncrRawMemCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetRawMemoryCount(), 1);

    GlobalResourceUsage::IncrRawMemCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetRawMemoryCount(), 2);

    GlobalResourceUsage::DecrRawMemCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetRawMemoryCount(), 1);

    GlobalResourceUsage::DecrRawMemCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetRawMemoryCount(), 0);

    GlobalResourceUsage::DecrRawMemCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetRawMemoryCount(), -1);

    GlobalResourceUsage::IncrRawMemCount("GlobalResourceUsageTest");
    EXPECT_EQ(GlobalResourceUsage::GetRawMemoryCount(), 0);
#endif
}
