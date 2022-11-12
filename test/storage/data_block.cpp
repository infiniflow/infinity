//
// Created by JinHai on 2022/11/12.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/data_block.h"
#include "main/profiler/base_profiler.h"

class DataBlockTest : public BaseTest {

};

TEST_F(DataBlockTest, test1) {
    using namespace infinity;

    DataBlock data_block;
    std::vector<DataType> column_types;
    column_types.emplace_back(LogicalType::kBoolean);

    size_t row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    // Test DataBlock::AppendValue
    for(size_t i = 0; i < row_count; ++ i) {
        data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
    }

    // Test DataBlock::GetValue
    for(size_t i = 0; i < row_count; ++ i) {
        Value value = data_block.GetValue(0, i);
        EXPECT_EQ(value.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(value.value_.boolean, (i % 2 == 0));
    }

    // Test DataBlock::Reset
    data_block.Reset();
    for(size_t i = 0; i < row_count; ++ i) {
        data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
    }

    for(size_t i = 0; i < row_count; ++ i) {
        Value value = data_block.GetValue(0, i);
        EXPECT_EQ(value.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(value.value_.boolean, (i % 2 == 0));
    }
}

TEST_F(DataBlockTest, test2) {
    using namespace infinity;

    DataBlock data_block;
    std::vector<DataType> column_types;
    column_types.emplace_back(LogicalType::kBoolean);

    size_t row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    // Test DataBlock::AppendValue
    for(size_t i = 0; i < row_count; ++ i) {
        data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
    }

    EXPECT_THROW(data_block.AppendValue(0, Value::MakeBool( (row_count + 1) % 2 == 0)), std::logic_error);
}

TEST_F(DataBlockTest, test3) {
    using namespace infinity;

    infinity::BaseProfiler profiler;

    profiler.Begin();

    DataBlock data_block;
    std::vector<DataType> column_types;
    column_types.emplace_back(LogicalType::kTinyInt);

    size_t row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    profiler.End();
    std::cout << "Initialize data block cost: " << profiler.ElapsedToString() << std::endl;

    // Test DataBlock::AppendValue
    profiler.Begin();
    for(size_t i = 0; i < row_count; ++ i) {
        data_block.AppendValue(0, Value::MakeTinyInt(i));
    }
    profiler.End();
    std::cout << "Append data to data block cost: " << profiler.ElapsedToString() << std::endl;
}