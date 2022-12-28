//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorBoolTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(ColumnVectorBoolTest, flat_boolean) {
    using namespace infinity;

    DataType data_type(LogicalType::kBoolean);
    ColumnVector col_target(data_type, ColumnVectorType::kFlat);
    col_target.Initialize();

    EXPECT_THROW(col_target.SetDataType(DataType(LogicalType::kBoolean)), TypeException);
    EXPECT_THROW(col_target.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_target.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_target.Size(), 0);
    EXPECT_THROW(col_target.ToString(), TypeException);
    EXPECT_THROW(col_target.GetValue(0), TypeException);
    EXPECT_EQ(col_target.tail_index_, 0);
    EXPECT_EQ(col_target.data_type_size_, 1);
    EXPECT_NE(col_target.data_ptr_, nullptr);
    EXPECT_EQ(col_target.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_target.data_type(), data_type);
    EXPECT_EQ(col_target.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_target.buffer_, nullptr);
    EXPECT_NE(col_target.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_target.initialized);
    col_target.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_target.data_ptr_;
    EXPECT_EQ(col_target.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_target.data_ptr_);

    {
        Value v = Value::MakeBool(true);
        EXPECT_THROW(col_target.SetValue(3, v), StorageException);
    }

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBool(static_cast<BooleanT>(i % 2 == 0));
        col_target.AppendValue(v);
        Value vx = col_target.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
        EXPECT_THROW(col_target.GetValue(i + 1), TypeException);
    }

    col_target.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_target.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
    }
    EXPECT_EQ(col_target.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_target.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBool(static_cast<BooleanT>(i % 2 == 0));
        col_target.AppendValue(v);
        Value vx = col_target.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
        EXPECT_THROW(col_target.GetValue(i + 1), TypeException);
    }

    col_target.Reset();
    EXPECT_EQ(col_target.capacity(), 0);
    EXPECT_EQ(col_target.tail_index_, 0);
//    EXPECT_EQ(col_target.data_type_size_, 0);
    EXPECT_NE(col_target.buffer_, nullptr);
    EXPECT_NE(col_target.data_ptr_, nullptr);
    EXPECT_EQ(col_target.initialized, false);
//    EXPECT_EQ(col_target.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_target.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_target.Initialize(), TypeException);
//    col_target.SetDataType(DataType(LogicalType::kBoolean));
//    EXPECT_THROW(col_target.Initialize(), TypeException);
//    col_target.SetVectorType(ColumnVectorType::kFlat);
    col_target.Initialize();
    EXPECT_THROW(col_target.SetDataType(DataType(LogicalType::kBoolean)), TypeException);
    EXPECT_THROW(col_target.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_target.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_target.Size(), 0);
    EXPECT_THROW(col_target.ToString(), TypeException);
    EXPECT_THROW(col_target.GetValue(0), TypeException);
    EXPECT_EQ(col_target.tail_index_, 0);
    EXPECT_EQ(col_target.data_type_size_, 1);
    EXPECT_NE(col_target.data_ptr_, nullptr);
    EXPECT_EQ(col_target.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_target.data_type(), data_type);
    EXPECT_EQ(col_target.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_target.buffer_, nullptr);
    EXPECT_NE(col_target.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_target.initialized);
    col_target.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_target.data_ptr_;
    EXPECT_EQ(col_target.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_target.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeBool(static_cast<BooleanT>(i % 2 == 0));
        col_target.AppendValue(v);
        Value vx = col_target.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
        EXPECT_THROW(col_target.GetValue(i + 1), TypeException);
    }
}