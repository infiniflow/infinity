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
        SkipList<Key, Value, Comparator>::Iterator iter = list.Find(i);
        if (iter.Valid()) {
            ASSERT_EQ(keys[i], iter.value());
            ASSERT_EQ(keys.count(i), 1);
        } else {
            ASSERT_EQ(keys.count(i), 0);
        }
    }
}

TEST_F(SkiplistTest, test2) {
    MemoryPool memory_pool;
    StringComparator cmp;
    SkipList<const char *, Value, StringComparator> list(cmp, &memory_pool);

    const int N = 20;
    const int R = 5000;
    std::map<String, Value> keys;

    Vector<String> strings;
    for (int i = 0; i < N; i++) {
        String key = RandStr();
        strings.push_back(key);
        Value value = rand() % R;
        if (keys.emplace(key, value).second) {
            list.Insert(strings[i].c_str(), value);
        }
    }

    for (std::map<String, Value>::iterator it = keys.begin(); it != keys.end(); ++it) {
        std::cout << "key1 " << it->first << " value " << it->second << std::endl;
        SkipList<const char *, Value, StringComparator>::Iterator iter = list.Find(it->first.c_str());
        if (iter.Valid()) {
            std::cout << "key " << iter.key() << std::endl;
            //  ASSERT_EQ(it->second, iter.value());
        }
    }
}