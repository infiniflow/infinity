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

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
#if 0

using namespace infinity;
class BlobTypeTest : public BaseTest {};

TEST_F(BlobTypeTest, blob1) {
    using namespace infinity;

    constexpr i64 SIZE = 27;

    auto blob_ptr = new char[SIZE]{0};

    for (i64 i = 0; i < SIZE; ++i) {
        blob_ptr[i] = 'a' + static_cast<char_t>(i);
    }
    blob_ptr[SIZE - 1] = 0;

    EXPECT_STREQ(blob_ptr, "abcdefghijklmnopqrstuvwxyz");

    // Constructor from ptr and size
    BlobT b1(blob_ptr, SIZE);
    EXPECT_STREQ(b1.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b1.size, SIZE);

    // Copy constructor
    BlobT b2(b1);
    EXPECT_STREQ(b2.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b2.size, SIZE);

    EXPECT_EQ(b1, b2);
    EXPECT_FALSE(b1 != b2);

    // std::move constructor
    BlobT b3(std::move(b1));
    EXPECT_STREQ(b2.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b2.size, SIZE);

    EXPECT_EQ(b1.ptr, nullptr);
    EXPECT_EQ(b1.size, 0);

    // Copy assignment
    BlobT b4;
    b4 = b2;
    EXPECT_STREQ(b4.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b4.size, SIZE);

    // std::move assignment
    BlobT b5;
    b5 = std::move(b2);
    EXPECT_STREQ(b5.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b5.size, SIZE);

    EXPECT_EQ(b2.ptr, nullptr);
    EXPECT_EQ(b2.size, 0);

    // Reset;
    b5.Reset();
    EXPECT_EQ(b5.ptr, nullptr);
    EXPECT_EQ(b5.size, 0);

    // std::move from ptr;
    blob_ptr = new char[SIZE]{0};

    for (i64 i = 0; i < SIZE; ++i) {
        blob_ptr[i] = 'a' + static_cast<char_t>(i);
    }
    blob_ptr[SIZE - 1] = 0;
    BlobT b6;
    b6.std::move(blob_ptr, SIZE);
    EXPECT_STREQ(b6.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b6.size, SIZE);

    // Copy from ptr;
    BlobT b7;
    b7.Copy(blob_ptr, SIZE);
    EXPECT_STREQ(b7.ptr, "abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(b7.size, SIZE);
}
#endif