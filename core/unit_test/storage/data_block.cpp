//
// Created by JinHai on 2022/11/12.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import parser;
import table_def;
import data_block;
import default_values;
import profiler;
import value;
import infinity;

class DataBlockTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(DataBlockTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    DataBlock data_block;
    Vector<SharedPtr<DataType>> column_types;
    // Bool * 1
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBoolean));

    // Integer * 5
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kSmallInt));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kInteger));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kHugeInt));

    // Float * 2
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kFloat));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));

    // Decimal * 1
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kDecimal, DecimalInfo::Make(38, 2)));

    // Varchar * 1
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kVarchar));

    // Date and Time * 6
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kDate));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kTime));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kDateTime));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kTimestamp));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kInterval));

    // Nested types * 2
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kArray, ArrayInfo::Make(DataType(LogicalType::kBigInt))));

    // TODO: how to handle Tuple type?
    // column_types.emplace_back(LogicalType::kTuple);

    // Geography * 7
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kPoint));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kLine));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kLineSeg));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBox));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kPath));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kPolygon));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kCircle));

    // Other * 4
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBitmap));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kUuid));
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBlob));

    // 32 dimension * float vector
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 32)));

    // Heterogeneous type * 1
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kMixed));

    SizeT row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    // Boolean: Test DataBlock::AppendValue
    constexpr SizeT BoolColumnIndex = 0;
    for (SizeT i = 0; i < row_count; ++i) {
        data_block.AppendValue(BoolColumnIndex, Value::MakeBool(i % 2 == 0));
    }

    // Boolean: Test DataBlock::GetValue
    for (SizeT i = 0; i < row_count; ++i) {
        Value value = data_block.GetValue(BoolColumnIndex, i);
        EXPECT_EQ(value.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(value.value_.boolean, (i % 2 == 0));
    }

    // TinyInt: Test DataBlock::AppendValue
    constexpr SizeT TinyIntColumnIndex = 1;
    for (SizeT i = 0; i < row_count; ++i) {
        data_block.AppendValue(TinyIntColumnIndex, Value::MakeTinyInt(static_cast<i8>(i)));
    }

    // Test DataBlock::GetValue
    for (SizeT i = 0; i < row_count; ++i) {
        Value value = data_block.GetValue(TinyIntColumnIndex, i);
        EXPECT_EQ(value.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(value.value_.tiny_int, i8(i));
    }

    // Test DataBlock::Reset
    data_block.Reset();
    for (SizeT i = 0; i < row_count; ++i) {
        data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
    }

    for (SizeT i = 0; i < row_count; ++i) {
        Value value = data_block.GetValue(0, i);
        EXPECT_EQ(value.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(value.value_.boolean, (i % 2 == 0));
    }
}

TEST_F(DataBlockTest, test2) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    DataBlock data_block;
    Vector<SharedPtr<DataType>> column_types;
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBoolean));

    SizeT row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    // Test DataBlock::AppendValue
    for (SizeT i = 0; i < row_count; ++i) {
        data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
    }
    EXPECT_THROW(data_block.AppendValue(1, Value::MakeBool(true)), StorageException);
    EXPECT_THROW(data_block.AppendValue(0, Value::MakeBool(true)), StorageException);

    EXPECT_FALSE(data_block.Finalized());
    data_block.Finalize();
    EXPECT_TRUE(data_block.Finalized());

    EXPECT_EQ(data_block.row_count(), row_count);

    SharedPtr<Vector<RowID>> row_ids = MakeShared<Vector<RowID>>();
    row_ids->reserve(row_count);
    data_block.FillRowIDVector(row_ids, 1);
    for (SizeT row_id = 0; row_id < row_count; ++row_id) {
        EXPECT_EQ((*row_ids)[row_id].block_offset_, row_id);
        EXPECT_EQ((*row_ids)[row_id].block_id_, 1);
        EXPECT_EQ((*row_ids)[row_id].segment_id_, INVALID_SEGMENT_ID);
    }
}

TEST_F(DataBlockTest, test3) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    infinity::BaseProfiler profiler;

    profiler.Begin();

    DataBlock data_block;
    Vector<SharedPtr<DataType>> column_types;
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));

    SizeT row_count = DEFAULT_VECTOR_SIZE;

    data_block.Init(column_types);

    profiler.End();
    std::cout << "Initialize data block cost: " << profiler.ElapsedToString() << std::endl;

    // Test to store value into invalid column
    EXPECT_THROW(data_block.SetValue(1, 0, Value::MakeTinyInt(static_cast<i8>(1))), StorageException);

    // Test to store value into valid column but invalid row
    EXPECT_THROW(data_block.SetValue(0, 1, Value::MakeTinyInt(static_cast<i8>(1))), StorageException);

    // Test DataBlock::AppendValue
    profiler.Begin();
    for (SizeT i = 0; i < row_count; ++i) {
        data_block.AppendValue(0, Value::MakeTinyInt(static_cast<i8>(i)));
    }
    profiler.End();
    std::cout << "Append data to data block cost: " << profiler.ElapsedToString() << std::endl;

    data_block.Finalize();

    // Validate the inserted data.
    for (SizeT i = 0; i < row_count; ++i) {
        auto v = Value::MakeTinyInt(static_cast<i8>(i));
        auto vx = data_block.GetValue(0, i);
        EXPECT_EQ(vx.type().type(), LogicalType::kTinyInt);
        EXPECT_EQ(vx.value_.tiny_int, v.value_.tiny_int);
    }
}

TEST_F(DataBlockTest, ReadWrite) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    DataBlock data_block;
    Vector<SharedPtr<DataType>> column_types;
    // Bool * 1
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kBoolean));

    // Integer * 5
    column_types.emplace_back(MakeShared<DataType>(LogicalType::kTinyInt));
    /*     column_types.emplace_back(MakeShared<DataType>(LogicalType::kSmallInt));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kInteger));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBigInt));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kHugeInt));

        // Float * 2
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kFloat));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kDouble));

        // Decimal * 1
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kDecimal, DecimalInfo::Make(38, 2)));

        // Varchar * 1
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kVarchar));

        // Date and Time * 6
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kDate));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTime));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kDateTime));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kTimestamp));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kInterval));

        // Nested types * 2
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kArray, ArrayInfo::Make(DataType(LogicalType::kBigInt))));

        // TODO: how to handle Tuple type?
        // column_types.emplace_back(LogicalType::kTuple);

        // Geography * 7
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kPoint));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kLine));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kLineSeg));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kBox));
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kPath));
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kPolygon));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kCircle));

        // Other * 4
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kBitmap));
        column_types.emplace_back(MakeShared<DataType>(LogicalType::kUuid));
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kBlob));

        // 32 dimension * float vector
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kEmbedding, EmbeddingInfo::Make(EmbeddingDataType::kElemFloat, 32)));

        // Heterogeneous type * 1
        //column_types.emplace_back(MakeShared<DataType>(LogicalType::kMixed));
     */
    SizeT row_count = DEFAULT_VECTOR_SIZE;
    data_block.Init(column_types);
    for (SizeT i = 0; i < row_count; ++i) {
        data_block.AppendValue(0, Value::MakeBool(i % 2 == 0));
        data_block.AppendValue(1, Value::MakeTinyInt(static_cast<i8>(i)));
    }
    data_block.Finalize();

    int32_t exp_size = data_block.GetSizeInBytes();
    Vector<char> buf(exp_size);
    char *ptr = buf.data();

    data_block.WriteAdv(ptr);
    EXPECT_EQ(ptr - buf.data(), exp_size);

    ptr = buf.data();
    SharedPtr<DataBlock> data_block2 = DataBlock::ReadAdv(ptr, exp_size);
    EXPECT_EQ(ptr - buf.data(), exp_size);
    EXPECT_NE(data_block2, nullptr);
    EXPECT_EQ(data_block == *data_block2, true);
}
