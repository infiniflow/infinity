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
#include <string.h>

import stl;
import logger;
import memory_pool;
import skiplist;

using namespace infinity;

class SkiplistTest : public BaseTest {};

typedef u64 Key;
typedef u64 Value;

struct Comparator {
    int operator()(const Key &a, const Key &b) const {
        if (a < b) {
            return -1;
        } else if (a > b) {
            return +1;
        } else {
            return 0;
        }
    }
};

struct StringComparator {
    int operator()(const String &lhs, const String &rhs) const {
        int ret = strcmp(lhs.c_str(), rhs.c_str());
        return ret < 0;
    }
};

String RandStr() {
    u32 len = rand() % 100;
    String str;
    str.reserve(len);
    for (u32 i = 0; i < len; ++i)
        str.push_back('a' + rand() % 26);
    return str;
}

TEST_F(SkiplistTest, test1) {
    MemoryPool memory_pool;
    Comparator cmp;
    SkipList<Key, Value, Comparator> list(cmp, &memory_pool);

    const int N = 2000;
    const int R = 5000;
    std::map<Key, Value> keys;

    for (int i = 0; i < N; i++) {
        Key key = rand() % R;
        Value value = rand() % R;
        if (keys.emplace(key, value).second) {
            list.Insert(key, value);
        }
    }

    for (int i = 0; i < R; i++) {
        // SkipList<Key, Value, Comparator>::Iterator iter = list.Find(i);
        Value v;
        if (list.Search(i, v)) {
            ASSERT_EQ(keys[i], v);
            ASSERT_EQ(keys.count(i), (unsigned)1);
        } else {
            ASSERT_EQ(keys.count(i), (unsigned)0);
        }
    }
}

TEST_F(SkiplistTest, test2) {
    MemoryPool memory_pool;
    StringComparator cmp;
    SkipList<String, Value, StringComparator> list(cmp, &memory_pool);

    const int N = 200;
    const int R = 5000;
    std::map<String, Value> keys;

    for (int i = 0; i < N; i++) {
        String key = RandStr();
        Value value = rand() % R;
        if (keys.emplace(key, value).second) {
            list.Insert(key, value);
        }
    }

    for (std::map<String, Value>::iterator it = keys.begin(); it != keys.end(); ++it) {
        Value v;
        if (list.Search(it->first, v)) {
            ASSERT_EQ(it->second, v);
        }
    }

    for (std::map<String, Value>::iterator it = keys.begin(); it != keys.end(); ++it) {
        SkipList<String, Value, StringComparator>::Iterator iter = list.Begin(it->first);
        if (iter != list.End()) {
            ASSERT_EQ(it->second, iter.Value());
        }
    }
}

const int NUM_THREADS = 4;
const int NUM_OPERATIONS = 1000;
Comparator cmp;
MemoryPool arena;
SkipList<int, int, Comparator> skipList(cmp, &arena);

void PerformOperations(int thread_id) {
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        if (thread_id == 0) {
            int key = rand() % 100;
            skipList.Insert(key, key);
            int result;
            bool ret = skipList.Search(key, result);
            ASSERT_EQ(ret, true) << "Insertion failed";
            ASSERT_EQ(result, key);
        } else {
            int key = rand() % 100;
            int result;
            bool ret = skipList.Search(key, result);
            if (ret) {
                ASSERT_EQ(result, key);
            }
        }
    }
}

TEST_F(SkiplistTest, test3) {
    std::thread threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = std::thread(PerformOperations, i);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }

    // You can add additional assertions here to validate the correctness of the SkipList
}