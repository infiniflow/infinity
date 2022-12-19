//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "common/types/info/varchar_info.h"

class ColumnVectorVarcharTest : public BaseTest {
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

TEST_F(ColumnVectorVarcharTest, flat_inline_varchar) {
    using namespace infinity;

    auto varchar_info = VarcharInfo::Make(65);
    DataType data_type(LogicalType::kVarchar, varchar_info);
    ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
    col_varchar.Initialize();

    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kVarchar)), TypeException);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), TypeException);
    EXPECT_THROW(col_varchar.GetValue(0), TypeException);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_NE(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "hello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        EXPECT_TRUE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }

    col_varchar.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        String s = "hello" + std::to_string(i);

        EXPECT_TRUE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
    }

    EXPECT_EQ(col_varchar.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {

        String s = "hello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        EXPECT_TRUE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }

    col_varchar.Reset();
    EXPECT_EQ(col_varchar.capacity(), 0);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.buffer_, nullptr);
    EXPECT_EQ(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.initialized, false);

    // ====
    col_varchar.Initialize();
    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), TypeException);
    EXPECT_THROW(col_varchar.GetValue(0), TypeException);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_NE(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "hello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        EXPECT_TRUE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorVarcharTest, flat_not_inline_varchar) {
    using namespace infinity;

    auto varchar_info = VarcharInfo::Make(65);
    DataType data_type(LogicalType::kVarchar, varchar_info);
    ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
    col_varchar.Initialize();

    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kVarchar)), TypeException);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), TypeException);
    EXPECT_THROW(col_varchar.GetValue(0), TypeException);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_NE(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "hellohellohello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        EXPECT_FALSE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }

    col_varchar.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        String s = "hellohellohello" + std::to_string(i);

        EXPECT_FALSE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
    }

    EXPECT_EQ(col_varchar.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {

        String s = "hellohellohello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        EXPECT_FALSE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }

    col_varchar.Reset();
    EXPECT_EQ(col_varchar.capacity(), 0);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.buffer_, nullptr);
    EXPECT_EQ(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.initialized, false);

    // ====
    col_varchar.Initialize();
    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), TypeException);
    EXPECT_THROW(col_varchar.GetValue(0), TypeException);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_NE(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "hellohellohello" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        EXPECT_FALSE(vx.value_.varchar.IsInlined());
        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorVarcharTest, flat_mixed_inline_varchar) {
    using namespace infinity;

    auto varchar_info = VarcharInfo::Make(65);
    DataType data_type(LogicalType::kVarchar, varchar_info);
    ColumnVector col_varchar(data_type, ColumnVectorType::kFlat);
    col_varchar.Initialize();

    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kVarchar)), TypeException);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), TypeException);
    EXPECT_THROW(col_varchar.GetValue(0), TypeException);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_NE(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "Professional" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);
        if(s.length() <= VarcharType::INLINE_LENGTH) {
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
        } else {
            EXPECT_FALSE(vx.value_.varchar.IsInlined());
        }

        if(vx.value_.varchar.IsInlined()) {
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }

    col_varchar.Reserve(DEFAULT_VECTOR_SIZE* 2);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        String s = "Professional" + std::to_string(i);

        if(s.length() <= VarcharType::INLINE_LENGTH) {
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            EXPECT_FALSE(vx.value_.varchar.IsInlined());
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
    }

    EXPECT_EQ(col_varchar.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {

        String s = "Professional" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        if(s.length() <= VarcharType::INLINE_LENGTH) {
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            EXPECT_FALSE(vx.value_.varchar.IsInlined());
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }

    col_varchar.Reset();
    EXPECT_EQ(col_varchar.capacity(), 0);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.buffer_, nullptr);
    EXPECT_EQ(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.initialized, false);

    // ====
    col_varchar.Initialize();
    EXPECT_THROW(col_varchar.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(col_varchar.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(col_varchar.Size(), 0);
    EXPECT_THROW(col_varchar.ToString(), TypeException);
    EXPECT_THROW(col_varchar.GetValue(0), TypeException);
    EXPECT_EQ(col_varchar.tail_index_, 0);
    EXPECT_EQ(col_varchar.data_type_size_, 16);
    EXPECT_NE(col_varchar.data_ptr_, nullptr);
    EXPECT_EQ(col_varchar.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(col_varchar.data_type(), data_type);
    EXPECT_EQ(col_varchar.buffer_->buffer_type_, VectorBufferType::kMemory);

    EXPECT_NE(col_varchar.buffer_, nullptr);
    EXPECT_NE(col_varchar.nulls_ptr_, nullptr);
    EXPECT_TRUE(col_varchar.initialized);
    col_varchar.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = col_varchar.data_ptr_;
    EXPECT_EQ(col_varchar.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, col_varchar.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        String s = "Professional" + std::to_string(i);
        VarcharT varchar_value(s);
        Value v = Value::MakeVarchar(varchar_value, varchar_info);
        col_varchar.AppendValue(v);
        Value vx = col_varchar.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kVarchar);

        if(s.length() <= VarcharType::INLINE_LENGTH) {
            EXPECT_TRUE(vx.value_.varchar.IsInlined());
            String prefix = String(vx.value_.varchar.prefix, vx.value_.varchar.length);
            EXPECT_STREQ(prefix.c_str(), s.c_str());
        } else {
            EXPECT_FALSE(vx.value_.varchar.IsInlined());
            String whole_str = String(vx.value_.varchar.ptr, vx.value_.varchar.length);
            EXPECT_STREQ(whole_str.c_str(), s.c_str());
        }
        EXPECT_THROW(col_varchar.GetValue(i + 1), TypeException);
    }
}
