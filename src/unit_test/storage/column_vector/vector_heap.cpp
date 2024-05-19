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
import var_heap;
import fix_heap;
import third_party;
import stl;
import global_resource_usage;
import infinity_context;
import default_values;
import vector_heap_chunk;

class VectorHeapTest : public BaseTest {};
#if 0
TEST_F(VectorHeapTest, var_heap1) {
    using namespace infinity;
    VarHeapManager vector_heap_mgr_;

    SizeT size_count = 20;
    SizeT allocate_size = 1;
    SizeT total_size = 0;

    for (SizeT i = 0; i < size_count; ++i) {
        allocate_size <<= 1;
        String data;
        data.reserve(allocate_size);
        for (u64 j = 0; j < allocate_size; ++j) {
            data.push_back(j % 26 + 'a');
        }
        auto [last_chunk_id, last_chunk_offset] = vector_heap_mgr_.AppendToHeap(data.c_str(), allocate_size);
        if(total_size < MAX_VECTOR_CHUNK_SIZE) {
            EXPECT_EQ(last_chunk_id, 0);
            EXPECT_EQ(last_chunk_offset, total_size);
        } else {
            EXPECT_EQ(last_chunk_id, total_size / MAX_VECTOR_CHUNK_SIZE);
            EXPECT_EQ(last_chunk_offset, total_size % MAX_VECTOR_CHUNK_SIZE);
        }

        String buffer;
        buffer.resize(allocate_size);
        vector_heap_mgr_.ReadFromHeap(buffer.data(), last_chunk_id, last_chunk_offset, allocate_size);
        EXPECT_EQ(data, buffer);
        total_size += allocate_size;
    }

    EXPECT_EQ(total_size, vector_heap_mgr_.total_size());

    std::cout << "total memory: " << vector_heap_mgr_.total_mem() << ", total size: " << total_size << std::endl;
    std::cout << vector_heap_mgr_.Stats() << std::endl;
}

TEST_F(VectorHeapTest, var_heap2) {
    using namespace infinity;
    VarHeapManager vector_heap_mgr_;

    SizeT size_count = 5000;
    SizeT allocate_size = 260;
    SizeT total_size = 0;

    for (SizeT i = 0; i < size_count; ++i) {
        String data;
        data.reserve(allocate_size);
        for (u64 j = 0; j < allocate_size; ++j) {
            data.push_back(j % 26 + 'a');
        }
        auto [last_chunk_id, last_chunk_offset] = vector_heap_mgr_.AppendToHeap(data.c_str(), allocate_size);
        if(total_size < MAX_VECTOR_CHUNK_SIZE) {
            EXPECT_EQ(last_chunk_id, 0);
            EXPECT_EQ(last_chunk_offset, total_size);
        } else {
            EXPECT_EQ(last_chunk_id, total_size / MAX_VECTOR_CHUNK_SIZE);
            EXPECT_EQ(last_chunk_offset, total_size % MAX_VECTOR_CHUNK_SIZE);
        }

        String buffer;
        buffer.resize(allocate_size);
        vector_heap_mgr_.ReadFromHeap(buffer.data(), last_chunk_id, last_chunk_offset, allocate_size);
        EXPECT_EQ(data, buffer);
        total_size += allocate_size;
    }

    EXPECT_EQ(total_size, vector_heap_mgr_.total_size());

    std::cout << "total memory: " << vector_heap_mgr_.total_mem() << ", total size: " << total_size << std::endl;
    std::cout << vector_heap_mgr_.Stats() << std::endl;
}
#endif

TEST_F(VectorHeapTest, fix_heap1) {
    using namespace infinity;
    FixHeapManager vector_heap_mgr_;

    SizeT size_count = 20;
    SizeT allocate_size = 1;
    SizeT total_size = 0;

    for (SizeT i = 0; i < size_count; ++i) {
        allocate_size <<= 1;
        String data;
        data.reserve(allocate_size);
        for (u64 j = 0; j < allocate_size; ++j) {
            data.push_back(j % 26 + 'a');
        }
        auto [last_chunk_id, last_chunk_offset] = vector_heap_mgr_.AppendToHeap(data.c_str(), allocate_size);
        EXPECT_EQ(last_chunk_id, ChunkId(total_size / DEFAULT_FIXLEN_CHUNK_SIZE));
        EXPECT_EQ(last_chunk_offset, total_size % DEFAULT_FIXLEN_CHUNK_SIZE);

        String buffer;
        buffer.resize(allocate_size);
        vector_heap_mgr_.ReadFromHeap(buffer.data(), last_chunk_id, last_chunk_offset, allocate_size);
        EXPECT_EQ(data, buffer);
        total_size += allocate_size;
    }

    EXPECT_EQ(total_size, vector_heap_mgr_.total_size());

    std::cout << "total memory: " << vector_heap_mgr_.total_mem() << ", total size: " << total_size << std::endl;
    std::cout << vector_heap_mgr_.Stats() << std::endl;
}

TEST_F(VectorHeapTest, fix_heap2) {
    using namespace infinity;
    FixHeapManager vector_heap_mgr_;

    SizeT size_count = 5000;
    SizeT allocate_size = 260;
    SizeT total_size = 0;

    for (SizeT i = 0; i < size_count; ++i) {
        String data;
        data.reserve(allocate_size);
        for (u64 j = 0; j < allocate_size; ++j) {
            data.push_back(j % 26 + 'a');
        }
        auto [last_chunk_id, last_chunk_offset] = vector_heap_mgr_.AppendToHeap(data.c_str(), allocate_size);
        EXPECT_EQ(last_chunk_id, ChunkId(total_size / DEFAULT_FIXLEN_CHUNK_SIZE));
        EXPECT_EQ(last_chunk_offset, total_size % DEFAULT_FIXLEN_CHUNK_SIZE);

        String buffer;
        buffer.resize(allocate_size);
        vector_heap_mgr_.ReadFromHeap(buffer.data(), last_chunk_id, last_chunk_offset, allocate_size);
        EXPECT_EQ(data, buffer);
        total_size += allocate_size;
    }

    EXPECT_EQ(total_size, vector_heap_mgr_.total_size());

    std::cout << "total memory: " << vector_heap_mgr_.total_mem() << ", total size: " << total_size << std::endl;
    std::cout << vector_heap_mgr_.Stats() << std::endl;
}


