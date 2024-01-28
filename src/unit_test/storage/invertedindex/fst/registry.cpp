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
import stl;
import fst;

using namespace infinity;

class RegistryTest : public ::testing::Test {
protected:
    Registry<int> registry{3};
};

TEST_F(RegistryTest, InsertAndFind) {
    registry.Insert(1, 100UL);
    registry.Insert(2, 200UL);
    registry.Insert(3, 300UL);

    EXPECT_EQ(registry.Find(1), 100UL);
    EXPECT_EQ(registry.Find(2), 200UL);
    EXPECT_EQ(registry.Find(3), 300UL);
}

TEST_F(RegistryTest, FindNonExistentKey) { EXPECT_EQ(registry.Find(4), 0UL); }

TEST_F(RegistryTest, InsertBeyondMaxSize) {
    registry.Insert(1, 100UL);
    EXPECT_EQ(registry.Size(), 1UL);
    registry.Insert(2, 200UL);
    EXPECT_EQ(registry.Size(), 2UL);
    registry.Insert(3, 300UL);
    EXPECT_EQ(registry.Size(), 3UL);
    registry.Insert(4, 400UL); // This should evict the first entry (1, 100)
    EXPECT_EQ(registry.Size(), 3UL);

    EXPECT_EQ(registry.Find(1), (SizeT)0); // Key 1 should have been evicted
    EXPECT_EQ(registry.Find(2), 200UL);
    EXPECT_EQ(registry.Find(3), 300UL);
    EXPECT_EQ(registry.Find(4), 400UL);

    registry.Clear();
    EXPECT_EQ(registry.Size(), 0UL);
}

TEST_F(RegistryTest, ReinsertKey) {
    registry.Insert(1, 100UL);
    registry.Insert(1, 200UL); // This should update the value of key 1

    EXPECT_EQ(registry.Find(1), 200UL);
}