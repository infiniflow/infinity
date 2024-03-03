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
        Value v;
        bool ret = list.Find(i, v);
        if (ret) {
            ASSERT_EQ(keys[i], v);
            ASSERT_EQ(keys.count(i), 1);
        } else {
            ASSERT_EQ(keys.count(i), 0);
        }
    }
}