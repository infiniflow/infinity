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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.test_low_cardinality;

import :ut.base_test;

import third_party;
import :secondary_index_data;
import :roaring_bitmap;
#endif

import data_type;
import logical_type;

using namespace infinity;

class LowCardinalitySecondaryIndexTest : public BaseTest {
public:
    LowCardinalitySecondaryIndexTest() = default;
    ~LowCardinalitySecondaryIndexTest() = default;

protected:
    // Helper function to create test data with low cardinality
    template <typename T>
    MultiMap<T, u32> CreateLowCardinalityData(u32 chunk_row_count, u32 unique_values = 5) {
        MultiMap<T, u32> test_data;

        // Create low cardinality data - many rows with same key values
        for (u32 i = 0; i < chunk_row_count; ++i) {
            T key = static_cast<T>(i % unique_values); // Only 'unique_values' distinct keys
            test_data.emplace(key, i);
        }

        return test_data;
    }
};

TEST_F(LowCardinalitySecondaryIndexTest, TestIntegerLowCardinality) {
    const u32 chunk_row_count = 100;
    const u32 unique_values = 5;

    // Create test data
    auto test_data = CreateLowCardinalityData<i32>(chunk_row_count, unique_values);

    // Create low cardinality secondary index
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto *index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, true);
    ASSERT_NE(index, nullptr);

    // Insert data
    index->InsertData(&test_data);

    // Verify unique key count
    EXPECT_EQ(index->GetUniqueKeyCount(), unique_values);

    // Test range queries
    const i32 *unique_keys = static_cast<const i32 *>(index->GetUniqueKeysPtr());
    ASSERT_NE(unique_keys, nullptr);

    // Verify unique keys are sorted
    for (u32 i = 1; i < unique_values; ++i) {
        EXPECT_LT(unique_keys[i - 1], unique_keys[i]);
    }

    // Test getting offsets for specific keys
    for (u32 i = 0; i < unique_values; ++i) {
        i32 key = static_cast<i32>(i);
        const auto *bitmap = static_cast<const Bitmap *>(index->GetOffsetsForKeyPtr(&key));
        ASSERT_NE(bitmap, nullptr);

        // Count expected offsets for this key
        u32 expected_count = 0;
        for (const auto &[data_key, offset] : test_data) {
            if (data_key == key) {
                expected_count++;
            }
        }

        // Count actual offsets in bitmap
        u32 actual_count = 0;
        bitmap->RoaringBitmapApplyFunc([&actual_count](u32 offset) -> bool {
            actual_count++;
            return true;
        });

        EXPECT_EQ(expected_count, actual_count);
    }

    delete index;
}

TEST_F(LowCardinalitySecondaryIndexTest, TestFloatLowCardinality) {
    const u32 chunk_row_count = 80;
    const u32 unique_values = 4;

    // Create test data with float keys
    MultiMap<float, u32> test_data;
    Vector<float> unique_float_keys = {1.5f, 2.5f, 3.5f, 4.5f};

    for (u32 i = 0; i < chunk_row_count; ++i) {
        float key = unique_float_keys[i % unique_values];
        test_data.emplace(key, i);
    }

    // Create low cardinality secondary index
    auto data_type = std::make_shared<DataType>(LogicalType::kFloat);
    auto *index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, true);

    // Insert data
    index->InsertData(&test_data);

    // Verify unique key count
    EXPECT_EQ(index->GetUniqueKeyCount(), unique_values);

    // Test specific key lookup
    for (u32 i = 0; i < unique_values; ++i) {
        float key = unique_float_keys[i];
        const auto *bitmap = static_cast<const Bitmap *>(index->GetOffsetsForKeyPtr(&key));
        ASSERT_NE(bitmap, nullptr);

        // Verify offsets for this key
        Vector<u32> actual_offsets;
        bitmap->RoaringBitmapApplyFunc([&actual_offsets](u32 offset) -> bool {
            actual_offsets.push_back(offset);
            return true;
        });

        // Expected offsets for this key
        Vector<u32> expected_offsets;
        for (const auto &[data_key, offset] : test_data) {
            if (data_key == key) {
                expected_offsets.push_back(offset);
            }
        }

        std::sort(actual_offsets.begin(), actual_offsets.end());
        std::sort(expected_offsets.begin(), expected_offsets.end());
        EXPECT_EQ(expected_offsets, actual_offsets);
    }

    delete index;
}

TEST_F(LowCardinalitySecondaryIndexTest, TestEmptyIndex) {
    const u32 chunk_row_count = 0;

    // Create empty test data
    MultiMap<i32, u32> test_data;

    // Create low cardinality secondary index
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto *index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, true);

    // Insert empty data
    index->InsertData(&test_data);

    // Verify empty index
    EXPECT_EQ(index->GetUniqueKeyCount(), 0);
    EXPECT_EQ(index->GetUniqueKeysPtr(), nullptr);

    delete index;
}

TEST_F(LowCardinalitySecondaryIndexTest, TestSingleValueCardinality) {
    const u32 chunk_row_count = 50;
    const u32 unique_values = 1;

    // Create test data with only one unique value
    auto test_data = CreateLowCardinalityData<i16>(chunk_row_count, unique_values);

    // Create low cardinality secondary index
    auto data_type = std::make_shared<DataType>(LogicalType::kSmallInt);
    auto *index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, true);

    // Insert data
    index->InsertData(&test_data);

    // Verify single unique key
    EXPECT_EQ(index->GetUniqueKeyCount(), 1);

    // Test the single key
    i16 key = 0;
    const auto *bitmap = static_cast<const Bitmap *>(index->GetOffsetsForKeyPtr(&key));
    ASSERT_NE(bitmap, nullptr);

    // All offsets should be in this single bitmap
    u32 offset_count = 0;
    bitmap->RoaringBitmapApplyFunc([&offset_count](u32 offset) -> bool {
        offset_count++;
        return true;
    });

    EXPECT_EQ(offset_count, chunk_row_count);

    delete index;
}

TEST_F(LowCardinalitySecondaryIndexTest, TestComparisonWithHighCardinality) {
    const u32 chunk_row_count = 100;
    const u32 unique_values = 5;

    // Create test data
    auto test_data = CreateLowCardinalityData<i32>(chunk_row_count, unique_values);

    // Create both high and low cardinality indexes
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto *high_card_index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, chunk_row_count, true);
    auto *low_card_index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, true);

    // Insert same data into both
    high_card_index->InsertData(&test_data);
    low_card_index->InsertData(&test_data);

    // High cardinality should have 0 unique keys (default implementation)
    EXPECT_EQ(high_card_index->GetUniqueKeyCount(), 0);
    EXPECT_EQ(high_card_index->GetUniqueKeysPtr(), nullptr);

    // Low cardinality should have the expected unique keys
    EXPECT_EQ(low_card_index->GetUniqueKeyCount(), unique_values);
    EXPECT_NE(low_card_index->GetUniqueKeysPtr(), nullptr);

    // Verify low cardinality index has correct data
    for (u32 i = 0; i < unique_values; ++i) {
        i32 key = static_cast<i32>(i);
        const auto *bitmap = static_cast<const Bitmap *>(low_card_index->GetOffsetsForKeyPtr(&key));
        ASSERT_NE(bitmap, nullptr);

        u32 expected_count = chunk_row_count / unique_values;
        u32 actual_count = 0;
        bitmap->RoaringBitmapApplyFunc([&actual_count](u32 offset) -> bool {
            actual_count++;
            return true;
        });

        EXPECT_EQ(expected_count, actual_count);
    }

    delete high_card_index;
    delete low_card_index;
}
