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
    registry.Insert(1, (SizeT)100);
    registry.Insert(2, (SizeT)200);
    registry.Insert(3, (SizeT)300);

    EXPECT_EQ(registry.Find(1), (SizeT)100);
    EXPECT_EQ(registry.Find(2), (SizeT)200);
    EXPECT_EQ(registry.Find(3), (SizeT)300);
}

TEST_F(RegistryTest, FindNonExistentKey) { EXPECT_EQ(registry.Find(4), 0); }

TEST_F(RegistryTest, InsertBeyondMaxSize) {
    registry.Insert(1, (SizeT)100);
    EXPECT_EQ(registry.Size(), 1);
    registry.Insert(2, (SizeT)200);
    EXPECT_EQ(registry.Size(), 2);
    registry.Insert(3, (SizeT)300);
    EXPECT_EQ(registry.Size(), 3);
    registry.Insert(4, (SizeT)400); // This should evict the first entry (1, 100)
    EXPECT_EQ(registry.Size(), 3);

    EXPECT_EQ(registry.Find(1), (SizeT)0); // Key 1 should have been evicted
    EXPECT_EQ(registry.Find(2), (SizeT)200);
    EXPECT_EQ(registry.Find(3), (SizeT)300);
    EXPECT_EQ(registry.Find(4), (SizeT)400);

    registry.Clear();
    EXPECT_EQ(registry.Size(), 0);
}

TEST_F(RegistryTest, ReinsertKey) {
    registry.Insert(1, (SizeT)100);
    registry.Insert(1, (SizeT)200); // This should update the value of key 1

    EXPECT_EQ(registry.Find(1), (SizeT)200);
}