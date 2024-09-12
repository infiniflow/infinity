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

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import logger;
import deprecated_bitmask_buffer;
import deprecated_bitmask;
import third_party;
import stl;

import infinity_context;
import global_resource_usage;

using namespace infinity;

class BitmaskBufferTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         BitmaskBufferTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

TEST_P(BitmaskBufferTest, bitmask_buffer_a) {
    using namespace infinity;

    constexpr size_t bit_count = 8192;

    {
        BitmaskBuffer bitmask_buffer;
        EXPECT_EQ(bitmask_buffer.count_, 0u);
        EXPECT_EQ(bitmask_buffer.data_ptr_, nullptr);

        EXPECT_THROW(bitmask_buffer.Initialize(7), UnrecoverableException);
        bitmask_buffer.Initialize(bit_count);
        EXPECT_EQ(bitmask_buffer.count_, bit_count);
        EXPECT_NE(bitmask_buffer.data_ptr_, nullptr);

        BitmaskBuffer bitmask_buffer1;
        EXPECT_EQ(bitmask_buffer1.count_, 0u);
        EXPECT_EQ(bitmask_buffer1.data_ptr_, nullptr);
        bitmask_buffer1.Initialize(bitmask_buffer.data_ptr_.get(), bit_count);
        EXPECT_EQ(bitmask_buffer1.count_, bit_count);
        EXPECT_NE(bitmask_buffer1.data_ptr_, nullptr);

        bitmask_buffer.Initialize(2 * bit_count);
        EXPECT_EQ(bitmask_buffer.count_, 2 * bit_count);
        EXPECT_NE(bitmask_buffer.data_ptr_, nullptr);
    }

    {
        auto bitmask_buffer_ptr = BitmaskBuffer::Make(bit_count);
        EXPECT_EQ(bitmask_buffer_ptr->count_, bit_count);
    }
}

TEST_P(BitmaskBufferTest, ReadWrite) {
    using namespace infinity;

    constexpr size_t bit_count = 8192;
    Vector<SharedPtr<Bitmask>> masks = {Bitmask::Make(bit_count), Bitmask::Make(bit_count), Bitmask::Make(bit_count)};
    masks[1]->SetAllFalse();
    masks[2]->SetFalse(4096);

    for (int i = 0; i < 3; i++) {
        auto &bitmask = masks[i];
        int32_t exp_size = bitmask->GetSizeInBytes();
        Vector<char> buf(exp_size);
        char *ptr = buf.data();

        bitmask->WriteAdv(ptr);
        EXPECT_EQ(ptr - buf.data(), exp_size);

        const char* ptr_r = buf.data();
        auto bitmask2 = Bitmask::ReadAdv(ptr_r, exp_size);
        EXPECT_EQ(ptr_r - buf.data(), exp_size);
        EXPECT_NE(bitmask2, nullptr);
        EXPECT_EQ(*bitmask, *bitmask2);
    }
}
