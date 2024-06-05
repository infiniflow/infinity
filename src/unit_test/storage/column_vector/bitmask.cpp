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

import logger;
import bitmask;
import third_party;
import stl;
import infinity_context;
import global_resource_usage;

class BitmaskTest : public BaseTest {
    void SetUp() override {
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }
};

TEST_F(BitmaskTest, bitmask_a) {
    using namespace infinity;

    constexpr size_t bit_count = 8192;

    Bitmask bitmask1;
    EXPECT_EQ(bitmask1.count(), 0u);
    EXPECT_EQ(bitmask1.GetData(), nullptr);

    // test Initialize()
    bitmask1.Initialize(bit_count);

    EXPECT_EQ(bitmask1.count(), bit_count);
    EXPECT_EQ(bitmask1.GetData(), nullptr);

    {
        Bitmask bitmask2;
        EXPECT_THROW(bitmask2.Initialize(bit_count - 1), UnrecoverableException);
    }

    // test DeepCopy()
    Bitmask bitmask2;
    bitmask2.DeepCopy(bitmask1);
    EXPECT_EQ(bitmask2.count(), bit_count);
    EXPECT_EQ(bitmask2.GetData(), nullptr);

    // test ShallowCopy()
    Bitmask bitmask3;
    bitmask3.ShallowCopy(bitmask1);
    EXPECT_EQ(bitmask3.count(), bit_count);
    EXPECT_EQ(bitmask3.GetData(), nullptr);

    // test IsAllTrue and IsTrue()
    EXPECT_TRUE(bitmask1.IsAllTrue());
    EXPECT_TRUE(bitmask2.IsAllTrue());
    EXPECT_TRUE(bitmask3.IsAllTrue());

    for (size_t i = 0; i < bit_count; ++i) {
        EXPECT_TRUE(bitmask1.IsTrue(i));
        EXPECT_TRUE(bitmask2.IsTrue(i));
        EXPECT_TRUE(bitmask3.IsTrue(i));
    }

    // test SetTrue and SetFalse()
    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            bitmask1.SetTrue(i);
        } else {
            bitmask1.SetFalse(i);
        }
    }

    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            EXPECT_TRUE(bitmask1.IsTrue(i));
        } else {
            EXPECT_FALSE(bitmask1.IsTrue(i));
        }
    }

    bitmask1.SetAllFalse();
    for (SizeT i = 0; i < bit_count; ++i) {
        EXPECT_FALSE(bitmask1.IsTrue(i));
    }

    EXPECT_EQ(bitmask1.CountFalse(), bit_count);
    EXPECT_EQ(bitmask1.CountTrue(), 0u);

    // test SetAllTrue
    bitmask1.SetAllTrue();

    for (size_t i = 0; i < bit_count; ++i) {
        EXPECT_TRUE(bitmask1.IsTrue(i));
    }

    // test CountTrue() and CountFalse()
    EXPECT_EQ(bitmask1.CountTrue(), bit_count);
    EXPECT_EQ(bitmask1.CountFalse(), 0u);

    for (size_t i = 0; i < bit_count; ++i) {
        bitmask1.Set(i, i % 2 == 0);
    }

    EXPECT_EQ(bitmask1.CountTrue(), bit_count / 2);
    EXPECT_EQ(bitmask1.CountFalse(), bit_count / 2);

    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            EXPECT_TRUE(bitmask1.IsTrue(i));
        } else {
            EXPECT_FALSE(bitmask1.IsTrue(i));
        }
    }

    // test Resize()
    bitmask1.Resize(2 * bit_count);
    EXPECT_EQ(bitmask1.count(), 2 * bit_count);
    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            EXPECT_TRUE(bitmask1.IsTrue(i));
        } else {
            EXPECT_FALSE(bitmask1.IsTrue(i));
        }
    }
    for (size_t i = bit_count; i < 2 * bit_count; ++i) {
        EXPECT_TRUE(bitmask1.IsTrue(i));
    }
    EXPECT_NE(bitmask1.GetData(), nullptr);

    {
        EXPECT_THROW(bitmask2.Resize(bit_count - 8), UnrecoverableException);
        EXPECT_THROW(bitmask2.Resize(bit_count - 1), UnrecoverableException);
    }

    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            bitmask2.Set(i, true);
            bitmask3.Set(i, false);
        } else {
            bitmask2.Set(i, false);
            bitmask3.Set(i, true);
        }
    }
    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            EXPECT_TRUE(bitmask2.IsTrue(i));
            EXPECT_FALSE(bitmask3.IsTrue(i));
        } else {
            EXPECT_FALSE(bitmask2.IsTrue(i));
            EXPECT_TRUE(bitmask3.IsTrue(i));
        }
    }
    bitmask2.Merge(bitmask3);
    EXPECT_EQ(bitmask2.CountTrue(), 0u);
}

TEST_F(BitmaskTest, bitmask_b) {
    using namespace infinity;

    constexpr size_t bit_count = 8192;
    SharedPtr<Bitmask> bitmask2 = Bitmask::Make(bit_count);

    for (size_t i = 0; i < bit_count; ++i) {
        bitmask2->Set(i, i % 2 == 0);
    }

    EXPECT_EQ(bitmask2->CountTrue(), bit_count / 2);
    EXPECT_EQ(bitmask2->CountFalse(), bit_count / 2);

    for (size_t i = 0; i < bit_count; ++i) {
        if (i % 2 == 0) {
            EXPECT_TRUE(bitmask2->IsTrue(i));
        } else {
            EXPECT_FALSE(bitmask2->IsTrue(i));
        }
    }
}
