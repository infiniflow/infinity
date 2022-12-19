//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class ColumnVectorCharTest : public BaseTest {
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

TEST_F(ColumnVectorCharTest, flat_char1) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar1);
    ColumnVector col_char1(data_type, ColumnVectorType::kFlat);
    col_char1.Initialize();

    EXPECT_THROW(col_char1.SetDataType(DataType(LogicalType::kChar1)), TypeException);
    EXPECT_THROW(col_char1.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char1.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char1.Size(), 0);
    EXPECT_THROW(col_char1.ToString(), TypeException);
    EXPECT_THROW(col_char1.GetValue(0), TypeException);
    EXPECT_EQ(col_char1.tail_index_, 0);
    EXPECT_EQ(col_char1.data_type_size_, 1);
    EXPECT_NE(col_char1.data_ptr_, nullptr);
    EXPECT_EQ(col_char1.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char1.data_type(), data_type);
    EXPECT_EQ(col_char1.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char1.buffer_, nullptr);
    EXPECT_NE(col_char1.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char1.initialized);
    col_char1.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char1.data_ptr_;
    EXPECT_EQ(col_char1.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char1.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char1T char1;
        char c = char(i);
        char1.Initialize(c);
        Value v = Value::MakeChar1(char1);
        col_char1.AppendValue(v);
        Value vx = col_char1.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar1);
        EXPECT_EQ(vx.value_.char1.value, char(i));
        EXPECT_THROW(col_char1.GetValue(i + 1), TypeException);
    }

    col_char1.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char1.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar1);
        EXPECT_EQ(vx.value_.char1.value, char(i));
    }


    EXPECT_EQ(col_char1.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char1.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char1T char1;
        char c = char(i);
        char1.Initialize(c);
        Value v = Value::MakeChar1(char1);
        col_char1.AppendValue(v);
        Value vx = col_char1.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar1);
        EXPECT_EQ(vx.value_.char1.value, char(i));
        EXPECT_THROW(col_char1.GetValue(i + 1), TypeException);
    }

    col_char1.Reset();
    EXPECT_EQ(col_char1.capacity(), 0);
    EXPECT_EQ(col_char1.tail_index_, 0);
    EXPECT_EQ(col_char1.buffer_, nullptr);
    EXPECT_EQ(col_char1.data_ptr_, nullptr);
    EXPECT_EQ(col_char1.initialized, false);

    // ====
    col_char1.Initialize();
    EXPECT_THROW(col_char1.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char1.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char1.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char1.Size(), 0);
    EXPECT_THROW(col_char1.ToString(), TypeException);
    EXPECT_THROW(col_char1.GetValue(0), TypeException);
    EXPECT_EQ(col_char1.tail_index_, 0);
    EXPECT_EQ(col_char1.data_type_size_, 1);
    EXPECT_NE(col_char1.data_ptr_, nullptr);
    EXPECT_EQ(col_char1.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char1.data_type(), data_type);
    EXPECT_EQ(col_char1.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char1.buffer_, nullptr);
    EXPECT_NE(col_char1.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char1.initialized);
    col_char1.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char1.data_ptr_;
    EXPECT_EQ(col_char1.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char1.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char1T char1;
        char c = char(i);
        char1.Initialize(c);
        Value v = Value::MakeChar1(char1);
        col_char1.AppendValue(v);
        Value vx = col_char1.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar1);
        EXPECT_EQ(vx.value_.char1.value, char(i));
        EXPECT_THROW(col_char1.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, flat_char2) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar2);
    ColumnVector col_char2(data_type, ColumnVectorType::kFlat);
    col_char2.Initialize();

    EXPECT_THROW(col_char2.SetDataType(DataType(LogicalType::kChar2)), TypeException);
    EXPECT_THROW(col_char2.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char2.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char2.Size(), 0);
    EXPECT_THROW(col_char2.ToString(), TypeException);
    EXPECT_THROW(col_char2.GetValue(0), TypeException);
    EXPECT_EQ(col_char2.tail_index_, 0);
    EXPECT_EQ(col_char2.data_type_size_, 2);
    EXPECT_NE(col_char2.data_ptr_, nullptr);
    EXPECT_EQ(col_char2.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char2.data_type(), data_type);
    EXPECT_EQ(col_char2.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char2.buffer_, nullptr);
    EXPECT_NE(col_char2.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char2.initialized);
    col_char2.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char2.data_ptr_;
    EXPECT_EQ(col_char2.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char2.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char2T char2;
        String s = std::to_string(i);
        char2.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar2(char2);
        col_char2.AppendValue(v);
        Value vx = col_char2.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar2);

        String validation_value(std::to_string(i).c_str(), 2);
        String char2_value(vx.value_.char2.value, 2);
        EXPECT_STREQ(char2_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char2.GetValue(i + 1), TypeException);
    }

    col_char2.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char2.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar2);

        String validation_value(std::to_string(i).c_str(), 2);
        String char2_value(vx.value_.char2.value, 2);
        EXPECT_STREQ(char2_value.c_str(), validation_value.c_str());
    }


    EXPECT_EQ(col_char2.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char2.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char2T char2;
        String s = std::to_string(i);
        char2.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar2(char2);
        col_char2.AppendValue(v);
        Value vx = col_char2.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar2);

        String validation_value(std::to_string(i).c_str(), 2);
        String char2_value(vx.value_.char2.value, 2);
        EXPECT_STREQ(char2_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char2.GetValue(i + 1), TypeException);
    }

    col_char2.Reset();
    EXPECT_EQ(col_char2.capacity(), 0);
    EXPECT_EQ(col_char2.tail_index_, 0);
    EXPECT_EQ(col_char2.buffer_, nullptr);
    EXPECT_EQ(col_char2.data_ptr_, nullptr);
    EXPECT_EQ(col_char2.initialized, false);

    // ====
    col_char2.Initialize();
    EXPECT_THROW(col_char2.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char2.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char2.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char2.Size(), 0);
    EXPECT_THROW(col_char2.ToString(), TypeException);
    EXPECT_THROW(col_char2.GetValue(0), TypeException);
    EXPECT_EQ(col_char2.tail_index_, 0);
    EXPECT_EQ(col_char2.data_type_size_, 2);
    EXPECT_NE(col_char2.data_ptr_, nullptr);
    EXPECT_EQ(col_char2.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char2.data_type(), data_type);
    EXPECT_EQ(col_char2.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char2.buffer_, nullptr);
    EXPECT_NE(col_char2.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char2.initialized);
    col_char2.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char2.data_ptr_;
    EXPECT_EQ(col_char2.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char2.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char2T char2;
        String s = std::to_string(i);
        char2.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar2(char2);
        col_char2.AppendValue(v);
        Value vx = col_char2.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar2);

        String validation_value(std::to_string(i).c_str(), 2);
        String char2_value(vx.value_.char2.value, 2);
        EXPECT_STREQ(char2_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char2.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, flat_char4) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar4);
    ColumnVector col_char4(data_type, ColumnVectorType::kFlat);
    col_char4.Initialize();

    EXPECT_THROW(col_char4.SetDataType(DataType(LogicalType::kChar4)), TypeException);
    EXPECT_THROW(col_char4.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char4.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char4.Size(), 0);
    EXPECT_THROW(col_char4.ToString(), TypeException);
    EXPECT_THROW(col_char4.GetValue(0), TypeException);
    EXPECT_EQ(col_char4.tail_index_, 0);
    EXPECT_EQ(col_char4.data_type_size_, 4);
    EXPECT_NE(col_char4.data_ptr_, nullptr);
    EXPECT_EQ(col_char4.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char4.data_type(), data_type);
    EXPECT_EQ(col_char4.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char4.buffer_, nullptr);
    EXPECT_NE(col_char4.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char4.initialized);
    col_char4.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char4.data_ptr_;
    EXPECT_EQ(col_char4.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char4.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char4T char4;
        String s = std::to_string(i);
        char4.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar4(char4);
        col_char4.AppendValue(v);
        Value vx = col_char4.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar4);

        String validation_value(std::to_string(i).c_str(), 4);
        String char4_value(vx.value_.char4.value, 4);
        EXPECT_STREQ(char4_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char4.GetValue(i + 1), TypeException);
    }

    col_char4.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char4.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar4);

        String validation_value(std::to_string(i).c_str(), 4);
        String char4_value(vx.value_.char4.value, 4);
        EXPECT_STREQ(char4_value.c_str(), validation_value.c_str());
    }


    EXPECT_EQ(col_char4.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char4.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char4T char4;
        String s = std::to_string(i);
        char4.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar4(char4);
        col_char4.AppendValue(v);
        Value vx = col_char4.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar4);

        String validation_value(std::to_string(i).c_str(), 4);
        String char4_value(vx.value_.char4.value, 4);
        EXPECT_STREQ(char4_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char4.GetValue(i + 1), TypeException);
    }

    col_char4.Reset();
    EXPECT_EQ(col_char4.capacity(), 0);
    EXPECT_EQ(col_char4.tail_index_, 0);
    EXPECT_EQ(col_char4.buffer_, nullptr);
    EXPECT_EQ(col_char4.data_ptr_, nullptr);
    EXPECT_EQ(col_char4.initialized, false);

    // ====
    col_char4.Initialize();
    EXPECT_THROW(col_char4.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char4.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char4.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char4.Size(), 0);
    EXPECT_THROW(col_char4.ToString(), TypeException);
    EXPECT_THROW(col_char4.GetValue(0), TypeException);
    EXPECT_EQ(col_char4.tail_index_, 0);
    EXPECT_EQ(col_char4.data_type_size_, 4);
    EXPECT_NE(col_char4.data_ptr_, nullptr);
    EXPECT_EQ(col_char4.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char4.data_type(), data_type);
    EXPECT_EQ(col_char4.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char4.buffer_, nullptr);
    EXPECT_NE(col_char4.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char4.initialized);
    col_char4.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char4.data_ptr_;
    EXPECT_EQ(col_char4.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char4.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char4T char4;
        String s = std::to_string(i);
        char4.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar4(char4);
        col_char4.AppendValue(v);
        Value vx = col_char4.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar4);

        String validation_value(std::to_string(i).c_str(), 4);
        String char4_value(vx.value_.char4.value, 4);
        EXPECT_STREQ(char4_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char4.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, flat_char8) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar8);
    ColumnVector col_char8(data_type, ColumnVectorType::kFlat);
    col_char8.Initialize();

    EXPECT_THROW(col_char8.SetDataType(DataType(LogicalType::kChar8)), TypeException);
    EXPECT_THROW(col_char8.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char8.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char8.Size(), 0);
    EXPECT_THROW(col_char8.ToString(), TypeException);
    EXPECT_THROW(col_char8.GetValue(0), TypeException);
    EXPECT_EQ(col_char8.tail_index_, 0);
    EXPECT_EQ(col_char8.data_type_size_, 8);
    EXPECT_NE(col_char8.data_ptr_, nullptr);
    EXPECT_EQ(col_char8.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char8.data_type(), data_type);
    EXPECT_EQ(col_char8.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char8.buffer_, nullptr);
    EXPECT_NE(col_char8.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char8.initialized);
    col_char8.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char8.data_ptr_;
    EXPECT_EQ(col_char8.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char8.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char8T char8;
        String s = std::to_string(i);
        char8.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar8(char8);
        col_char8.AppendValue(v);
        Value vx = col_char8.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar8);

        String validation_value(std::to_string(i).c_str(), 8);
        String char8_value(vx.value_.char8.value, 8);
        EXPECT_STREQ(char8_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char8.GetValue(i + 1), TypeException);
    }

    col_char8.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char8.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar8);

        String validation_value(std::to_string(i).c_str(), 8);
        String char8_value(vx.value_.char8.value, 8);
        EXPECT_STREQ(char8_value.c_str(), validation_value.c_str());
    }


    EXPECT_EQ(col_char8.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char8.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char8T char8;
        String s = std::to_string(i);
        char8.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar8(char8);
        col_char8.AppendValue(v);
        Value vx = col_char8.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar8);

        String validation_value(std::to_string(i).c_str(), 8);
        String char8_value(vx.value_.char8.value, 8);
        EXPECT_STREQ(char8_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char8.GetValue(i + 1), TypeException);
    }

    col_char8.Reset();
    EXPECT_EQ(col_char8.capacity(), 0);
    EXPECT_EQ(col_char8.tail_index_, 0);
    EXPECT_EQ(col_char8.buffer_, nullptr);
    EXPECT_EQ(col_char8.data_ptr_, nullptr);
    EXPECT_EQ(col_char8.initialized, false);

    // ====
    col_char8.Initialize();
    EXPECT_THROW(col_char8.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char8.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char8.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char8.Size(), 0);
    EXPECT_THROW(col_char8.ToString(), TypeException);
    EXPECT_THROW(col_char8.GetValue(0), TypeException);
    EXPECT_EQ(col_char8.tail_index_, 0);
    EXPECT_EQ(col_char8.data_type_size_, 8);
    EXPECT_NE(col_char8.data_ptr_, nullptr);
    EXPECT_EQ(col_char8.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char8.data_type(), data_type);
    EXPECT_EQ(col_char8.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char8.buffer_, nullptr);
    EXPECT_NE(col_char8.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char8.initialized);
    col_char8.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char8.data_ptr_;
    EXPECT_EQ(col_char8.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char8.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char8T char8;
        String s = std::to_string(i);
        char8.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar8(char8);
        col_char8.AppendValue(v);
        Value vx = col_char8.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar8);

        String validation_value(std::to_string(i).c_str(), 8);
        String char8_value(vx.value_.char8.value, 8);
        EXPECT_STREQ(char8_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char8.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, flat_char16) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar16);
    ColumnVector col_char16(data_type, ColumnVectorType::kFlat);
    col_char16.Initialize();

    EXPECT_THROW(col_char16.SetDataType(DataType(LogicalType::kChar16)), TypeException);
    EXPECT_THROW(col_char16.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char16.Size(), 0);
    EXPECT_THROW(col_char16.ToString(), TypeException);
    EXPECT_THROW(col_char16.GetValue(0), TypeException);
    EXPECT_EQ(col_char16.tail_index_, 0);
    EXPECT_EQ(col_char16.data_type_size_, 16);
    EXPECT_NE(col_char16.data_ptr_, nullptr);
    EXPECT_EQ(col_char16.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char16.data_type(), data_type);
    EXPECT_EQ(col_char16.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char16.buffer_, nullptr);
    EXPECT_NE(col_char16.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char16.initialized);
    col_char16.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char16.data_ptr_;
    EXPECT_EQ(col_char16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char16.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char16T char16;
        String s = std::to_string(i);
        char16.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar16(char16);
        col_char16.AppendValue(v);
        Value vx = col_char16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar16);

        String validation_value(std::to_string(i).c_str(), 16);
        String char16_value(vx.value_.char16.value, 16);
        EXPECT_STREQ(char16_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char16.GetValue(i + 1), TypeException);
    }

    col_char16.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar16);

        String validation_value(std::to_string(i).c_str(), 16);
        String char16_value(vx.value_.char16.value, 16);
        EXPECT_STREQ(char16_value.c_str(), validation_value.c_str());
    }


    EXPECT_EQ(col_char16.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char16.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char16T char16;
        String s = std::to_string(i);
        char16.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar16(char16);
        col_char16.AppendValue(v);
        Value vx = col_char16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar16);

        String validation_value(std::to_string(i).c_str(), 16);
        String char16_value(vx.value_.char16.value, 16);
        EXPECT_STREQ(char16_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char16.GetValue(i + 1), TypeException);
    }

    col_char16.Reset();
    EXPECT_EQ(col_char16.capacity(), 0);
    EXPECT_EQ(col_char16.tail_index_, 0);
    EXPECT_EQ(col_char16.buffer_, nullptr);
    EXPECT_EQ(col_char16.data_ptr_, nullptr);
    EXPECT_EQ(col_char16.initialized, false);

    // ====
    col_char16.Initialize();
    EXPECT_THROW(col_char16.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char16.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char16.Size(), 0);
    EXPECT_THROW(col_char16.ToString(), TypeException);
    EXPECT_THROW(col_char16.GetValue(0), TypeException);
    EXPECT_EQ(col_char16.tail_index_, 0);
    EXPECT_EQ(col_char16.data_type_size_, 16);
    EXPECT_NE(col_char16.data_ptr_, nullptr);
    EXPECT_EQ(col_char16.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char16.data_type(), data_type);
    EXPECT_EQ(col_char16.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char16.buffer_, nullptr);
    EXPECT_NE(col_char16.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char16.initialized);
    col_char16.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char16.data_ptr_;
    EXPECT_EQ(col_char16.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char16.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char16T char16;
        String s = std::to_string(i);
        char16.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar16(char16);
        col_char16.AppendValue(v);
        Value vx = col_char16.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar16);

        String validation_value(std::to_string(i).c_str(), 16);
        String char16_value(vx.value_.char16.value, 16);
        EXPECT_STREQ(char16_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char16.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, flat_char32) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar32);
    ColumnVector col_char32(data_type, ColumnVectorType::kFlat);
    col_char32.Initialize();

    EXPECT_THROW(col_char32.SetDataType(DataType(LogicalType::kChar32)), TypeException);
    EXPECT_THROW(col_char32.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char32.Size(), 0);
    EXPECT_THROW(col_char32.ToString(), TypeException);
    EXPECT_THROW(col_char32.GetValue(0), TypeException);
    EXPECT_EQ(col_char32.tail_index_, 0);
    EXPECT_EQ(col_char32.data_type_size_, 32);
    EXPECT_NE(col_char32.data_ptr_, nullptr);
    EXPECT_EQ(col_char32.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char32.data_type(), data_type);
    EXPECT_EQ(col_char32.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char32.buffer_, nullptr);
    EXPECT_NE(col_char32.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char32.initialized);
    col_char32.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char32.data_ptr_;
    EXPECT_EQ(col_char32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char32.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char32T char32;
        String s = std::to_string(i);
        char32.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar32(char32);
        col_char32.AppendValue(v);
        Value vx = col_char32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar32);

        String validation_value(std::to_string(i).c_str(), 32);
        String char32_value(vx.value_.char32.value, 32);
        EXPECT_STREQ(char32_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char32.GetValue(i + 1), TypeException);
    }

    col_char32.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar32);

        String validation_value(std::to_string(i).c_str(), 32);
        String char32_value(vx.value_.char32.value, 32);
        EXPECT_STREQ(char32_value.c_str(), validation_value.c_str());
    }


    EXPECT_EQ(col_char32.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char32.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char32T char32;
        String s = std::to_string(i);
        char32.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar32(char32);
        col_char32.AppendValue(v);
        Value vx = col_char32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar32);

        String validation_value(std::to_string(i).c_str(), 32);
        String char32_value(vx.value_.char32.value, 32);
        EXPECT_STREQ(char32_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char32.GetValue(i + 1), TypeException);
    }

    col_char32.Reset();
    EXPECT_EQ(col_char32.capacity(), 0);
    EXPECT_EQ(col_char32.tail_index_, 0);
    EXPECT_EQ(col_char32.buffer_, nullptr);
    EXPECT_EQ(col_char32.data_ptr_, nullptr);
    EXPECT_EQ(col_char32.initialized, false);

    // ====
    col_char32.Initialize();
    EXPECT_THROW(col_char32.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char32.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char32.Size(), 0);
    EXPECT_THROW(col_char32.ToString(), TypeException);
    EXPECT_THROW(col_char32.GetValue(0), TypeException);
    EXPECT_EQ(col_char32.tail_index_, 0);
    EXPECT_EQ(col_char32.data_type_size_, 32);
    EXPECT_NE(col_char32.data_ptr_, nullptr);
    EXPECT_EQ(col_char32.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char32.data_type(), data_type);
    EXPECT_EQ(col_char32.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char32.buffer_, nullptr);
    EXPECT_NE(col_char32.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char32.initialized);
    col_char32.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char32.data_ptr_;
    EXPECT_EQ(col_char32.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char32.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char32T char32;
        String s = std::to_string(i);
        char32.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar32(char32);
        col_char32.AppendValue(v);
        Value vx = col_char32.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar32);

        String validation_value(std::to_string(i).c_str(), 32);
        String char32_value(vx.value_.char32.value, 32);
        EXPECT_STREQ(char32_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char32.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, flat_char64) {
    using namespace infinity;

    DataType data_type(LogicalType::kChar64);
    ColumnVector col_char64(data_type, ColumnVectorType::kFlat);
    col_char64.Initialize();

    EXPECT_THROW(col_char64.SetDataType(DataType(LogicalType::kChar64)), TypeException);
    EXPECT_THROW(col_char64.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char64.Size(), 0);
    EXPECT_THROW(col_char64.ToString(), TypeException);
    EXPECT_THROW(col_char64.GetValue(0), TypeException);
    EXPECT_EQ(col_char64.tail_index_, 0);
    EXPECT_EQ(col_char64.data_type_size_, 64);
    EXPECT_NE(col_char64.data_ptr_, nullptr);
    EXPECT_EQ(col_char64.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char64.data_type(), data_type);
    EXPECT_EQ(col_char64.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char64.buffer_, nullptr);
    EXPECT_NE(col_char64.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char64.initialized);
    col_char64.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_char64.data_ptr_;
    EXPECT_EQ(col_char64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char64.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char64T char64;
        String s = std::to_string(i);
        char64.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar64(char64);
        col_char64.AppendValue(v);
        Value vx = col_char64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar64);

        String validation_value(std::to_string(i).c_str(), 64);
        String char64_value(vx.value_.char64.value, 64);
        EXPECT_STREQ(char64_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char64.GetValue(i + 1), TypeException);
    }

    col_char64.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_char64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar64);

        String validation_value(std::to_string(i).c_str(), 64);
        String char64_value(vx.value_.char64.value, 64);
        EXPECT_STREQ(char64_value.c_str(), validation_value.c_str());
    }


    EXPECT_EQ(col_char64.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char64.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Char64T char64;
        String s = std::to_string(i);
        char64.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar64(char64);
        col_char64.AppendValue(v);
        Value vx = col_char64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar64);

        String validation_value(std::to_string(i).c_str(), 64);
        String char64_value(vx.value_.char64.value, 64);
        EXPECT_STREQ(char64_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char64.GetValue(i + 1), TypeException);
    }

    col_char64.Reset();
    EXPECT_EQ(col_char64.capacity(), 0);
    EXPECT_EQ(col_char64.tail_index_, 0);
    EXPECT_EQ(col_char64.buffer_, nullptr);
    EXPECT_EQ(col_char64.data_ptr_, nullptr);
    EXPECT_EQ(col_char64.initialized, false);

    // ====
    col_char64.Initialize();
    EXPECT_THROW(col_char64.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_char64.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_char64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_char64.Size(), 0);
    EXPECT_THROW(col_char64.ToString(), TypeException);
    EXPECT_THROW(col_char64.GetValue(0), TypeException);
    EXPECT_EQ(col_char64.tail_index_, 0);
    EXPECT_EQ(col_char64.data_type_size_, 64);
    EXPECT_NE(col_char64.data_ptr_, nullptr);
    EXPECT_EQ(col_char64.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_char64.data_type(), data_type);
    EXPECT_EQ(col_char64.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(col_char64.buffer_, nullptr);
    EXPECT_NE(col_char64.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_char64.initialized);
    col_char64.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_char64.data_ptr_;
    EXPECT_EQ(col_char64.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_char64.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Char64T char64;
        String s = std::to_string(i);
        char64.Initialize(s.c_str(), s.length());
        Value v = Value::MakeChar64(char64);
        col_char64.AppendValue(v);
        Value vx = col_char64.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar64);

        String validation_value(std::to_string(i).c_str(), 64);
        String char64_value(vx.value_.char64.value, 64);
        EXPECT_STREQ(char64_value.c_str(), validation_value.c_str());

        EXPECT_THROW(col_char64.GetValue(i + 1), TypeException);
    }
}
