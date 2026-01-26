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

module infinity_core:ut.test_secondary_index_merge;

import :ut.base_test;

import third_party;
import :secondary_index_data;
import :secondary_index_file_worker;
import :virtual_store;
import :file_worker;

import data_type;
import logical_type;

using namespace infinity;

// Mock FileWorker for HighCardinalityTag
class MockHighCardFileWorker : public FileWorker {
public:
    explicit MockHighCardFileWorker(SecondaryIndexDataBase<HighCardinalityTag>* index_data, u32 row_count)
        : FileWorker(std::make_shared<std::string>("mock_highcard_path")),
          index_data_(index_data),
          row_count_(row_count) {}
    
    ~MockHighCardFileWorker() override = default;
    
    FileWorkerType Type() const override {
        return FileWorkerType::kSecondaryIndexFile;
    }
    
    void Read(SecondaryIndexDataBase<HighCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) override {
        data = index_data_.get();
    }
    
    void Read(SecondaryIndexDataBase<LowCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) override {
        // Not used for this mock
        data = nullptr;
    }
    
    u32 GetRowCount() const { return row_count_; }
    
private:
    std::unique_ptr<SecondaryIndexDataBase<HighCardinalityTag>> index_data_;
    u32 row_count_;
};

// Mock FileWorker for LowCardinalityTag
class MockLowCardFileWorker : public FileWorker {
public:
    explicit MockLowCardFileWorker(SecondaryIndexDataBase<LowCardinalityTag>* index_data, u32 row_count)
        : FileWorker(std::make_shared<std::string>("mock_lowcard_path")),
          index_data_(index_data),
          row_count_(row_count) {}
    
    ~MockLowCardFileWorker() override = default;
    
    FileWorkerType Type() const override {
        return FileWorkerType::kSecondaryIndexFile;
    }
    
    void Read(SecondaryIndexDataBase<HighCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) override {
        // Not used for this mock
        data = nullptr;
    }
    
    void Read(SecondaryIndexDataBase<LowCardinalityTag> *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) override {
        data = index_data_.get();
    }
    
    u32 GetRowCount() const { return row_count_; }
    
private:
    std::unique_ptr<SecondaryIndexDataBase<LowCardinalityTag>> index_data_;
    u32 row_count_;
};

class SecondaryIndexMergeTest : public BaseTestNoParam {
public:
    SecondaryIndexMergeTest() = default;
    ~SecondaryIndexMergeTest() = default;

protected:
    template <typename T>
    std::multimap<T, u32> CreateTestData(u32 chunk_row_count, u32 unique_values, u32 offset_start = 0) {
        std::multimap<T, u32> test_data;

        for (u32 i = 0; i < chunk_row_count; ++i) {
            T key = static_cast<T>((i + offset_start) % unique_values);
            test_data.emplace(key, i);
        }

        return test_data;
    }
    
    // Helper to create a mock FileWorker with test data for HighCardinality
    template <typename T>
    MockHighCardFileWorker* CreateMockHighCardFileWorker(const std::multimap<T, u32>& test_data) {
        auto data_type = std::make_shared<DataType>(GetLogicalType<T>);
        u32 chunk_row_count = test_data.size();
        
        auto* index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, chunk_row_count, true);
        
        // Convert test data to the right type for insertion
        std::multimap<ConvertToOrderedType<T>, u32> converted_data;
        for (const auto& [key, offset] : test_data) {
            converted_data.emplace(ConvertToOrderedKeyValue(key), offset);
        }
        
        index->InsertData(&converted_data);
        
        return new MockHighCardFileWorker(index, chunk_row_count);
    }
    
    // Helper to create a mock FileWorker with test data for LowCardinality
    template <typename T>
    MockLowCardFileWorker* CreateMockLowCardFileWorker(const std::multimap<T, u32>& test_data) {
        auto data_type = std::make_shared<DataType>(GetLogicalType<T>);
        u32 chunk_row_count = test_data.size();
        
        auto* index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, chunk_row_count, true);
        
        // Convert test data to the right type for insertion
        std::multimap<ConvertToOrderedType<T>, u32> converted_data;
        for (const auto& [key, offset] : test_data) {
            converted_data.emplace(ConvertToOrderedKeyValue(key), offset);
        }
        
        index->InsertData(&converted_data);
        
        return new MockLowCardFileWorker(index, chunk_row_count);
    }
};

// Test InsertMergeData for HighCardinality
TEST_F(SecondaryIndexMergeTest, TestHighCardinalityMerge) {
    // Create three chunks of test data
    const u32 chunk1_row_count = 10;
    const u32 chunk2_row_count = 15;
    const u32 chunk3_row_count = 12;
    const u32 unique_values = 5;
    
    auto data1 = CreateTestData<i32>(chunk1_row_count, unique_values, 0);
    auto data2 = CreateTestData<i32>(chunk2_row_count, unique_values, 3); // Different offset
    auto data3 = CreateTestData<i32>(chunk3_row_count, unique_values, 7); // Different offset
    
    // Create mock FileWorkers
    auto* mock_worker1 = CreateMockHighCardFileWorker(data1);
    auto* mock_worker2 = CreateMockHighCardFileWorker(data2);
    auto* mock_worker3 = CreateMockHighCardFileWorker(data3);
    
    // Create target index with total row count
    u32 total_row_count = chunk1_row_count + chunk2_row_count + chunk3_row_count;
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto* target_index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, total_row_count, true);
    
    // Prepare vector for InsertMergeData
    std::vector<std::pair<u32, FileWorker*>> old_chunks = {
        {chunk1_row_count, mock_worker1},
        {chunk2_row_count, mock_worker2},
        {chunk3_row_count, mock_worker3}
    };
    
    // Call InsertMergeData
    target_index->InsertMergeData(old_chunks);
    
    // Verify the index contains correct data
    auto [key_ptr, offset_ptr] = target_index->GetKeyOffsetPointer();
    EXPECT_NE(key_ptr, nullptr);
    EXPECT_NE(offset_ptr, nullptr);
    
    // Verify we can search for keys
    for (u32 i = 0; i < unique_values; ++i) {
        i32 key = static_cast<i32>(i);
        auto result = target_index->SearchPGM(&key);
        // result is SecondaryIndexApproxPos, check if pos is reasonable
        EXPECT_LE(result.pos_, total_row_count);
        EXPECT_LE(result.lower_bound_, result.upper_bound_);
        EXPECT_LE(result.upper_bound_, total_row_count);
    }
    
    // Cleanup
    delete target_index;
    delete mock_worker1;
    delete mock_worker2;
    delete mock_worker3;
}

// Test InsertMergeData for LowCardinality
TEST_F(SecondaryIndexMergeTest, TestLowCardinalityMerge) {
    // Create two chunks of test data
    const u32 chunk1_row_count = 20;
    const u32 chunk2_row_count = 25;
    const u32 unique_values = 3;
    
    auto data1 = CreateTestData<i32>(chunk1_row_count, unique_values, 0);
    auto data2 = CreateTestData<i32>(chunk2_row_count, unique_values, 2); // Different offset
    
    // Create mock FileWorkers
    auto* mock_worker1 = CreateMockLowCardFileWorker(data1);
    auto* mock_worker2 = CreateMockLowCardFileWorker(data2);
    
    // Create target index with total row count
    u32 total_row_count = chunk1_row_count + chunk2_row_count;
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto* target_index = GetSecondaryIndexDataWithCardinality<LowCardinalityTag>(data_type, total_row_count, true);
    
    // Prepare vector for InsertMergeData
    std::vector<std::pair<u32, FileWorker*>> old_chunks = {
        {chunk1_row_count, mock_worker1},
        {chunk2_row_count, mock_worker2}
    };
    
    // Call InsertMergeData
    target_index->InsertMergeData(old_chunks);
    
    // Verify unique key count
    EXPECT_EQ(target_index->GetUniqueKeyCount(), unique_values);
    
    // Verify we can get offsets for each unique key
    for (u32 i = 0; i < unique_values; ++i) {
        i32 key = static_cast<i32>(i);
        const void* bitmap_ptr = target_index->GetOffsetsForKeyPtr(&key);
        EXPECT_NE(bitmap_ptr, nullptr);
    }
    
    // Cleanup
    delete target_index;
    delete mock_worker1;
    delete mock_worker2;
}

// Test InsertMergeData with empty chunks
TEST_F(SecondaryIndexMergeTest, TestEmptyMerge) {
    auto data_type = std::make_shared<DataType>(LogicalType::kInteger);
    auto* target_index = GetSecondaryIndexDataWithCardinality<HighCardinalityTag>(data_type, 0, true);
    
    std::vector<std::pair<u32, FileWorker*>> old_chunks;
    
    // Should not crash
    target_index->InsertMergeData(old_chunks);
    
    delete target_index;
}