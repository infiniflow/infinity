// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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
import stl;
import lru;

using namespace infinity;

class LRUTest : public BaseTest {};

TEST_F(LRUTest, test1) {
    lru_cache<String, String> cache(3);
    cache.set("a", "1");
    cache.set("b", "2");
    cache.set("c", "3");
    EXPECT_EQ(cache.maybe_get("a").value(), "1");
    EXPECT_EQ(cache.maybe_get("b").value(), "2");
    EXPECT_EQ(cache.maybe_get("c").value(), "3");
    cache.set("d", "4");                           // This should evict "a"
    EXPECT_EQ(cache.maybe_get("a"), std::nullopt); // "a" should be evicted
    EXPECT_EQ(cache.maybe_get("b").value(), "2");
    EXPECT_EQ(cache.maybe_get("c").value(), "3");
    EXPECT_EQ(cache.maybe_get("d").value(), "4");
    cache.set("b", "5"); // This should update "b" and move it to the front
    EXPECT_EQ(cache.maybe_get("b").value(), "5");
    cache.set("e", "6");                           // This should evict "c"
    EXPECT_EQ(cache.maybe_get("c"), std::nullopt); // "c" should be evicted
    EXPECT_EQ(cache.maybe_get("d").value(), "4");
    EXPECT_EQ(cache.maybe_get("e").value(), "6");
    cache.set("f", "7");                           // This should evict "d"
    EXPECT_EQ(cache.maybe_get("d").value(), "4"); // "d" should be evicted
    EXPECT_EQ(cache.maybe_get("e").value(), "6");
    EXPECT_EQ(cache.maybe_get("f").value(), "7");

    cache.unset("e"); // This should remove "e" from the cache
    EXPECT_EQ(cache.maybe_get("d").value(), "4"); // "d" should be evicted
    EXPECT_EQ(cache.maybe_get("e"), std::nullopt);
    EXPECT_EQ(cache.maybe_get("f").value(), "7");

    EXPECT_EQ(cache.size(), 3); // Cache should have 3 items left
}