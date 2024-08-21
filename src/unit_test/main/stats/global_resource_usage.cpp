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
import global_resource_usage;

class GlobalResourceUsageTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
    }

    void TearDown() override {
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(GlobalResourceUsageTest, usage_test) {
    using namespace infinity;

    // Object count
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

}