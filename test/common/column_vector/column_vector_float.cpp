//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorFloatTest : public BaseTest {
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

TEST_F(ColumnVectorFloatTest, flat_float) {
    using namespace infinity;

    DataType data_type(LogicalType::kFloat);
    ColumnVector col_float(data_type, ColumnVectorType::kFlat);
    col_float.Initialize();

    EXPECT_THROW(col_float.SetDataType(DataType(LogicalType::kFloat)), std::logic_error);
    EXPECT_THROW(col_float.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_float.Size(), 0);
    EXPECT_THROW(col_float.ToString(), std::logic_error);
    EXPECT_THROW(col_float.GetValue(0), std::logic_error);
    EXPECT_EQ(col_float.tail_index_, 0);
    EXPECT_EQ(col_float.data_type_size_, 4);
    EXPECT_NE(col_float.data_ptr_, nullptr);
    EXPECT_EQ(col_float.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_float.data_type(), data_type);
    EXPECT_EQ(col_float.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_float.buffer_, nullptr);
    EXPECT_EQ(col_float.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_float.initialized);
    col_float.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_float.data_ptr_;
    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_float.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        col_float.AppendValue(v);
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(col_float.GetValue(i + 1), std::logic_error);
    }

    col_float.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
    }
    EXPECT_EQ(col_float.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_float.capacity(), 2* DEFAULT_VECTOR_SIZE);
//    return ;
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        col_float.AppendValue(v);
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(col_float.GetValue(i + 1), std::logic_error);
    }

    col_float.Reset();
    EXPECT_EQ(col_float.capacity(), 0);
    EXPECT_EQ(col_float.tail_index_, 0);
//    EXPECT_EQ(col_float.data_type_size_, 0);
    EXPECT_EQ(col_float.buffer_, nullptr);
    EXPECT_EQ(col_float.data_ptr_, nullptr);
    EXPECT_EQ(col_float.initialized, false);
//    EXPECT_EQ(col_float.data_type(), DataType(LogicalType::kInvalid));
//    EXPECT_EQ(col_float.vector_type(), ColumnVectorType::kInvalid);

    // ====
//    EXPECT_THROW(col_float.Initialize(), std::logic_error);
//    col_float.SetDataType(DataType(LogicalType::kFloat));
//    EXPECT_THROW(col_float.Initialize(), std::logic_error);
//    col_float.SetVectorType(ColumnVectorType::kFlat);
    col_float.Initialize();
    EXPECT_THROW(col_float.SetDataType(DataType(LogicalType::kFloat)), std::logic_error);
    EXPECT_THROW(col_float.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_float.Size(), 0);
    EXPECT_THROW(col_float.ToString(), std::logic_error);
    EXPECT_THROW(col_float.GetValue(0), std::logic_error);
    EXPECT_EQ(col_float.tail_index_, 0);
    EXPECT_EQ(col_float.data_type_size_, 4);
    EXPECT_NE(col_float.data_ptr_, nullptr);
    EXPECT_EQ(col_float.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_float.data_type(), data_type);
    EXPECT_EQ(col_float.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_float.buffer_, nullptr);
    EXPECT_EQ(col_float.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_float.initialized);
    col_float.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_float.data_ptr_;
    EXPECT_EQ(col_float.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_float.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        col_float.AppendValue(v);
        Value vx = col_float.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(col_float.GetValue(i + 1), std::logic_error);
    }
}

TEST_F(ColumnVectorFloatTest, flat_double) {
    using namespace infinity;

    DataType data_type(LogicalType::kDouble);
    ColumnVector col_double(data_type, ColumnVectorType::kFlat);
    col_double.Initialize();

    EXPECT_THROW(col_double.SetDataType(DataType(LogicalType::kDouble)), std::logic_error);
    EXPECT_THROW(col_double.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_double.Size(), 0);
    EXPECT_THROW(col_double.ToString(), std::logic_error);
    EXPECT_THROW(col_double.GetValue(0), std::logic_error);
    EXPECT_EQ(col_double.tail_index_, 0);
    EXPECT_EQ(col_double.data_type_size_, 8);
    EXPECT_NE(col_double.data_ptr_, nullptr);
    EXPECT_EQ(col_double.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_double.data_type(), data_type);
    EXPECT_EQ(col_double.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_double.buffer_, nullptr);
    EXPECT_EQ(col_double.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_double.initialized);
    col_double.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_double.data_ptr_;
    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_double.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
    }

    col_double.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
    }
    EXPECT_EQ(col_double.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_double.capacity(), 2* DEFAULT_VECTOR_SIZE);
    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
    }

    col_double.Reset();
    EXPECT_EQ(col_double.capacity(), 0);
    EXPECT_EQ(col_double.tail_index_, 0);
    EXPECT_EQ(col_double.buffer_, nullptr);
    EXPECT_EQ(col_double.data_ptr_, nullptr);
    EXPECT_EQ(col_double.initialized, false);

    // ====
    col_double.Initialize();
    EXPECT_THROW(col_double.SetDataType(DataType(LogicalType::kDouble)), std::logic_error);
    EXPECT_THROW(col_double.SetVectorType(ColumnVectorType::kFlat), std::logic_error);

    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_double.Size(), 0);
    EXPECT_THROW(col_double.ToString(), std::logic_error);
    EXPECT_THROW(col_double.GetValue(0), std::logic_error);
    EXPECT_EQ(col_double.tail_index_, 0);
    EXPECT_EQ(col_double.data_type_size_, 8);
    EXPECT_NE(col_double.data_ptr_, nullptr);
    EXPECT_EQ(col_double.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_double.data_type(), data_type);
    EXPECT_EQ(col_double.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_double.buffer_, nullptr);
    EXPECT_EQ(col_double.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_double.initialized);
    col_double.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_double.data_ptr_;
    EXPECT_EQ(col_double.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_double.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        col_double.AppendValue(v);
        Value vx = col_double.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(col_double.GetValue(i + 1), std::logic_error);
    }
}


