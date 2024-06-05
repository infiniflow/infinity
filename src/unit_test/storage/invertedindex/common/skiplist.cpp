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
#include <random>

import stl;
import logger;

import skiplist;

using namespace infinity;

class SkiplistTest : public BaseTest {};

typedef u32 Key;
typedef u32 Value;

static String available_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

static String RandStr() {
    u32 len = random() % 100 + 50;
    String ret(len, '\0');
    for (SizeT i = 0; i < len; i++) {
        SizeT rand_i = random() % available_chars.size();
        ret[i] = available_chars[rand_i];
    }
    return ret;
}

class NumComparator {
public:
    i32 operator()(const int &lhs, const int &rhs) const {
        if (lhs < rhs)
            return -1;
        else if (lhs == rhs)
            return 0;
        else
            return 1;
    }
};

TEST_F(SkiplistTest, test1) {

    NumComparator cmp;
    SkipList<Key, Value, NumComparator> list(cmp);

    constexpr int N = 50000;
    constexpr int R = 50000;
    std::map<Key, Value> keys;

    Key a = 512;
    Value b = 111;
    list.Insert(a, b);
    Value v;
    list.Search(a, v);
    ASSERT_EQ(b, v);

    for (int i = 0; i < N; i++) {
        Key key = random() % R;
        Value value = random() % R;
        if (keys.emplace(key, value).second) {
            list.Insert(key, value);

            Value v;
            list.Search(key, v);
            ASSERT_EQ(value, v);
        }
    }

    for (std::map<Key, Value>::iterator it = keys.begin(); it != keys.end(); ++it) {
        Value v;
        if (list.Search(it->first, v)) {
            ASSERT_EQ(it->second, v);
        }
    }

    for (std::map<Key, Value>::iterator it = keys.begin(); it != keys.end(); ++it) {
        SkipList<Key, Value, NumComparator>::Iterator iter = list.Begin(it->first);
        if (iter != list.End()) {
            ASSERT_EQ(it->second, iter.Value());
        }
    }
}

TEST_F(SkiplistTest, test2) {
    KeyComparator cmp;
    SkipList<String, String, KeyComparator> list(cmp);
    constexpr int N = 200;
    std::map<String, String> keys;

    for (int i = 0; i < N; i++) {
        String key = RandStr() + std::to_string(i);
        if (keys.emplace(key, key).second) {
            list.Insert(key, key);

            String v;
            auto ret = list.Search(key, v);
            ASSERT_EQ(ret, true);
            ASSERT_EQ(v, key);
        }
    }

    for (std::map<String, String>::iterator it = keys.begin(); it != keys.end(); ++it) {
        String v;
        auto ret = list.Search(it->first, v);
        ASSERT_EQ(ret, true);
        ASSERT_EQ(it->second, v);
    }
    for (std::map<String, String>::iterator it = keys.begin(); it != keys.end(); ++it) {
        SkipList<String, String, KeyComparator>::Iterator iter = list.Begin(it->first);
        if (iter != list.End()) {
            ASSERT_EQ(it->second, iter.Value());
        }
    }
}

std::vector<int> num_list;
void InitRamdomList(int num_elements, int max_value) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(max_value / 2, max_value);

    for (int i = 0; i < num_elements; i++) {
        num_list.push_back(dis(gen));
    }
}

constexpr int NUM_THREADS = 4;
constexpr int NUM_OPERATIONS = 1000;
KeyComparator cmp;
SkipList<String, String, KeyComparator> s(cmp);
void PerformOperations(int thread_id) {
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
        if (thread_id == 0) {
            String key = RandStr();
            s.Insert(key, key);

            String result;
            bool ret = s.Search(key, result);
            ASSERT_EQ(ret, true);
            ASSERT_EQ(result, key);

        } else {
            String key = RandStr();
            String result;
            bool ret = s.Search(key, result);
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