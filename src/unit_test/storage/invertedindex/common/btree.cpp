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
#include <iostream>
#include <storage/invertedindex/common/vespa_alloc.h>
#include <vespalib/btree/btree.h>
#include <vespalib/btree/btree.hpp>
#include <vespalib/btree/btreenodeallocator.h>
#include <vespalib/btree/btreeroot.h>

import stl;
import third_party;
import logger;
import memory_pool;

class BtreeTest : public BaseTest {};

TEST_F(BtreeTest, test1) {
    using namespace infinity;
    MemoryPool memory_pool;
    SharedPtr<vespalib::alloc::MemoryPoolAllocator> memory_allocator = MakeShared<vespalib::alloc::MemoryPoolAllocator>(&memory_pool);
    Btree<u32, u32> btree(memory_allocator.get());
    btree.insert(1, 2);
    btree.insert(3, 4);
    btree.insert(5, 6);
    btree.insert(7, 8);
    btree.insert(9, 10);
    btree.insert(11, 12);
    btree.insert(13, 14);
    btree.insert(15, 16);
    auto iter = btree.begin();
    auto i = 0;
    for (; iter.valid(); ++iter, ++i) {
        EXPECT_EQ(iter.getKey(), 2 * i + 1);
        EXPECT_EQ(iter.getData(), 2 * i + 2);
    }
}
