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

import stl;
import third_party;
import logger;
import memory_pool;
import lock_free_vector;

class LockFreeVectorTest : public BaseTest {};

TEST_F(LockFreeVectorTest, test1) {
    using namespace infinity;
    MemoryPool memory_pool;
    LockFreeVector<u32> vec(&memory_pool);
    vec.PushBack(1);
    vec.PushBack(2);
    vec.PushBack(3);
    vec.PushBack(4);
    vec.PushBack(5);
    EXPECT_EQ(vec.Size(), 5);
    for (u32 i = 0; i < vec.Size(); ++i) {
        EXPECT_EQ(vec[i], i + 1);
    }
}

void PushTest(infinity::LockFreeVector<infinity::u32> *vec, int number_of_pushes, bool lock) {
    for (int i = 0; i < number_of_pushes; i++) {
        vec->PushBack(i, lock);
    }
}

void ReadTest(infinity::LockFreeVector<infinity::u32> *vec) {
    for (int i = 0; i < vec->Size(); i++) {
        auto v = vec->At(i);
    }
}

TEST_F(LockFreeVectorTest, test2) {
    using namespace infinity;
    MemoryPool memory_pool;
    LockFreeVector<u32> vec(&memory_pool);
    int number_of_threads = 4;
    int number_of_pushes = 100;

    Thread push_thread_list[number_of_threads];
    for (int i = 0; i < number_of_threads; i++) {
        push_thread_list[i] = Thread(PushTest, &vec, number_of_pushes, true);
    }
    for (int i = 0; i < number_of_threads; i++) {
        push_thread_list[i].join();
    }
    EXPECT_EQ(vec.Size(), (number_of_pushes * number_of_threads));

    Vector<u32> stl_vec;
    for (int i = 0; i < vec.Size(); i++) {
        stl_vec.push_back(vec[i]);
    }

    std::sort(stl_vec.begin(), stl_vec.end());

    // Test that there are number_of_threads copies of each value in the vector
    for (int i = 0; i < stl_vec.size() - number_of_threads; i += number_of_threads) {
        for (int j = i + 1; j < i + number_of_threads; j++) {
            EXPECT_EQ(stl_vec[j], stl_vec[i]);
        }
    }
}

TEST_F(LockFreeVectorTest, test3) {
    using namespace infinity;
    MemoryPool memory_pool;
    LockFreeVector<u32> vec(&memory_pool);
    int number_of_threads = 16;
    int number_of_pushes = 100;

    Thread push_thread = Thread(PushTest, &vec, number_of_pushes, false);
    Thread read_thread_list[number_of_threads];
    for (int i = 0; i < number_of_threads; i++) {
        read_thread_list[i] = Thread(ReadTest, &vec);
    }
    push_thread.join();
    for (int i = 0; i < number_of_threads; i++) {
        read_thread_list[i].join();
    }
}