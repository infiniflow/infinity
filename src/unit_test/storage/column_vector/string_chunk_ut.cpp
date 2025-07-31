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
#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"

module infinity_core:ut.string_chunk;

import :ut.base_test;
import :infinity_exception;
import :logger;
import :third_party;
import :stl;
import :infinity_context;
import :default_values;
#endif

import global_resource_usage;

#if 0
using namespace infinity;

class StringChunkTest : public BaseTest {};

TEST_F(StringChunkTest, string_chunk_a) {
    using namespace infinity;

    StringHeapMgr string_chunk_mgr;

    EXPECT_THROW_WITHOUT_STACKTRACE(string_chunk_mgr.Allocate(0), UnrecoverableException);

    for (u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26u);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192u);
}
#endif
