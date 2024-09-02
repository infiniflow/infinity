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
import selection;
import default_values;
import third_party;
import stl;
import global_resource_usage;
import infinity_context;

using namespace infinity;

class SelectionTest : public BaseTest {};

TEST_F(SelectionTest, test1) {
    using namespace infinity;

    Selection s1;
    EXPECT_THROW(s1.Append(1), UnrecoverableException);
    EXPECT_THROW(s1.Size(), UnrecoverableException);
    EXPECT_THROW(s1.Capacity(), UnrecoverableException);

    EXPECT_THROW(s1.Initialize(std::numeric_limits<u16>::max() + 1), UnrecoverableException);

    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        EXPECT_EQ(s1.Get(i), i);
    }

    s1.Initialize();
    EXPECT_EQ(s1.Size(), 0u);
    EXPECT_EQ(s1.Capacity(), u64(DEFAULT_VECTOR_SIZE));
    EXPECT_THROW(s1.Get(DEFAULT_VECTOR_SIZE), UnrecoverableException);
    EXPECT_THROW(s1.Get(0), UnrecoverableException);

    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        s1.Append(i * 2);
    }
    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        EXPECT_EQ(s1.Get(i), 2 * i);
    }
    EXPECT_EQ(s1.Size(), (u64)DEFAULT_VECTOR_SIZE);
    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        s1[i] = 3 * i;
    }
    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        EXPECT_EQ(s1.Get(i), 3 * i);
    }
    EXPECT_THROW(s1.Get(DEFAULT_VECTOR_SIZE), UnrecoverableException);
    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        s1.Set(i, 4 * i);
    }
    for (SizeT i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        EXPECT_EQ(s1.Get(i), 4 * i);
    }
}
