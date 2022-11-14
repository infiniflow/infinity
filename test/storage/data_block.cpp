//
// Created by JinHai on 2022/11/12.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/data_block.h"
#include "common/types/info/decimal_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/array_info.h"
#include "common/types/info/embedding_info.h"
#include "main/profiler/base_profiler.h"

class DataBlockTest : public BaseTest {
    void
    SetUp() override {
    }

    void
    TearDown() override {
    }
};

TEST_F(DataBlockTest, test1) {
    using namespace infinity;

    DataBlock data_block;
    std::vector<DataType> column_types;
    // Bool * 1
    column_types.emplace_back(LogicalType::kBoolean);

    // Integer * 5
    column_types.emplace_back(LogicalType::kTinyInt);
    column_types.emplace_back(LogicalType::kSmallInt);
    column_types.emplace_back(LogicalType::kInteger);
    column_types.emplace_back(LogicalType::kBigInt);
    column_types.emplace_back(LogicalType::kHugeInt);

    // Float * 2
    column_types.emplace_back(LogicalType::kFloat);
    column_types.emplace_back(LogicalType::kDouble);

    // Decimal * 4
    column_types.emplace_back(LogicalType::kDecimal16, DecimalInfo::Make(4, 2));
    column_types.emplace_back(LogicalType::kDecimal32, DecimalInfo::Make(9, 2));
    column_types.emplace_back(LogicalType::kDecimal64, DecimalInfo::Make(18, 2));
    column_types.emplace_back(LogicalType::kDecimal128, DecimalInfo::Make(38, 2));

    EXPECT_THROW(column_types.emplace_back(LogicalType::kVarchar, VarcharInfo::Make(16)), TypeException);

    // Varchar * 1
    column_types.emplace_back(LogicalType::kVarchar, VarcharInfo::Make(64));

    // Char * 7
    column_types.emplace_back(LogicalType::kChar1);
    column_types.emplace_back(LogicalType::kChar2);
    column_types.emplace_back(LogicalType::kChar4);
    column_types.emplace_back(LogicalType::kChar8);
    column_types.emplace_back(LogicalType::kChar15);
    column_types.emplace_back(LogicalType::kChar31);
    column_types.emplace_back(LogicalType::kChar63);

    // Date and Time * 6
    column_types.emplace_back(LogicalType::kDate);
    column_types.emplace_back(LogicalType::kTime);
    column_types.emplace_back(LogicalType::kDateTime);
    column_types.emplace_back(LogicalType::kTimestamp);
    column_types.emplace_back(LogicalType::kTimestampTZ);
    column_types.emplace_back(LogicalType::kInterval);

    // Nested types * 2
    column_types.emplace_back(LogicalType::kArray, ArrayInfo::Make(DataType(LogicalType::kBigInt)));

    // TODO: how to handle Tuple type?
    // column_types.emplace_back(LogicalType::kTuple);

    // Geography * 7
    column_types.emplace_back(LogicalType::kPoint);
    column_types.emplace_back(LogicalType::kLine);
    column_types.emplace_back(LogicalType::kLineSeg);
    column_types.emplace_back(LogicalType::kBox);
    column_types.emplace_back(LogicalType::kPath);
    column_types.emplace_back(LogicalType::kPolygon);
    column_types.emplace_back(LogicalType::kCircle);

    // Other * 4
    column_types.emplace_back(LogicalType::kBitmap);
    column_types.emplace_back(LogicalType::kUuid);
    column_types.emplace_back(LogicalType::kBlob);

    // 32 dimension * float vector
    column_types.emplace_back(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 32));

    // Heterogeneous type * 1
    column_types.emplace_back(LogicalType::kMixed);

    size_t row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    // Boolean: Test DataBlock::AppendValue
    constexpr size_t BoolColumnIndex = 0;
    for(size_t i = 0; i < row_count; ++ i) {
        data_block.AppendValue(BoolColumnIndex, Value::MakeBool(i % 2 == 0));
    }

    // Boolean: Test DataBlock::GetValue
    for(size_t i = 0; i < row_count; ++ i) {
        Value value = data_block.GetValue(BoolColumnIndex, i);
        EXPECT_EQ(value.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(value.value_.boolean, (i % 2 == 0));
    }

    // TinyInt: Test DataBlock::AppendValue
    constexpr size_t TinyIntColumnIndex = 1;
    for(size_t i = 0; i < row_count; ++ i) {
        data_block.AppendValue(TinyIntColumnIndex, Value::MakeTinyInt(i));
    }

    // Test DataBlock::GetValue
    for(size_t i = 0; i < row_count; ++ i) {
        Value value = data_block.GetValue(TinyIntColumnIndex, i);
        EXPECT_EQ(value.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(value.value_.tiny_int, i8(i));
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