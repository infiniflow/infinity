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

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_high_cardinality;

import :ut.base_test;

import third_party;
import :secondary_index_data;
import :roaring_bitmap;

import data_type;
import logical_type;

using namespace infinity;

class HighCardinalitySecondaryIndexTest : public BaseTest {
public:
    HighCardinalitySecondaryIndexTest() = default;
    ~HighCardinalitySecondaryIndexTest() = default;

protected:
    template <typename T>
    std::multimap<T, u32> CreateHighCardinalityData(u32 chunk_row_count, u32 unique_values = 5) {
        std::multimap<T, u32> test_data;

        for (u32 i = 0; i < chunk_row_count; ++i) {
            T key = static_cast<T>(i % unique_values);
            test_data.emplace(key, i);
        }

        return test_data;
    }
};

TEST_F(HighCardinalitySecondaryIndexTest, TestSaveLoadHighCardinality) {
    const u32 chunk_row_count = 10;
    const u32 unique_values = 5;

    auto test_data = CreateHighCardinalityData<i32>(chunk_row_count, unique_values);

    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto *index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, chunk_row_count, true);

    index->InsertData(&test_data);
    auto key_count = index->GetUniqueKeyCount();

    // Save data
    std::string tmp_path = fmt::format("{}/{}", "/var/infinity/tmp", "test001.idx");
    auto [file, status] = VirtualStore::Open(tmp_path, FileAccessMode::kWrite);
    EXPECT_TRUE(status.ok());
    index->SaveIndexInner(*file);

    auto *index2 = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, chunk_row_count, true);

    // Load data
    auto [file2, status2] = VirtualStore::Open(tmp_path, FileAccessMode::kRead);
    EXPECT_TRUE(status2.ok());
    index2->ReadIndexInner(*file2);
    EXPECT_EQ(key_count, index2->GetUniqueKeyCount());
}