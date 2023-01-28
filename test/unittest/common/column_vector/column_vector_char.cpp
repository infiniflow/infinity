//
// Created by JinHai on 2022/12/4.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "common/types/info/char_info.h"

class ColumnVectorCharTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(ColumnVectorCharTest, flat_char) {
    using namespace infinity;

    auto char_info = CharInfo::Make(128);
    DataType data_type(LogicalType::kChar, char_info);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(DataType(LogicalType::kEmbedding)), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 128);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    auto tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());

        v.value_.char_n.Reset();
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE* 2);

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.tail_index_, clone_column_vector.tail_index_);
    EXPECT_EQ(column_vector.capacity_, clone_column_vector.capacity_);
    EXPECT_EQ(column_vector.data_type_, clone_column_vector.data_type_);
    EXPECT_EQ(column_vector.data_ptr_, clone_column_vector.data_ptr_);
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type_, clone_column_vector.vector_type_);

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2* DEFAULT_VECTOR_SIZE);

    for(i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());

        v.value_.char_n.Reset();
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(DataType(LogicalType::kDecimal128)), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 128);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1);
    tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);
    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());

        v.value_.char_n.Reset();
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, contant_char) {

    using namespace infinity;

    auto char_info = CharInfo::Make(128);
    DataType data_type(LogicalType::kChar, char_info);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(1, ColumnVectorType::kConstant);

    EXPECT_THROW(column_vector.SetDataType(DataType(LogicalType::kChar)), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), TypeException);

    EXPECT_EQ(column_vector.capacity(), 1);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 128);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), StorageException);
    auto tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(column_vector.capacity(), 1);
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);

    for(i64 i = 0; i < 1; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), StorageException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());

        v.value_.char_n.Reset();
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }
    for (i64 i = 0; i < 1; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
//    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(1, ColumnVectorType::kConstant);
    EXPECT_THROW(column_vector.SetDataType(DataType(LogicalType::kChar)), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), TypeException);

    EXPECT_EQ(column_vector.capacity(), 1);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 128);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    EXPECT_THROW(column_vector.Reserve(DEFAULT_VECTOR_SIZE - 1), StorageException);
    tmp_ptr = column_vector.data_ptr_;
    EXPECT_EQ(tmp_ptr, column_vector.data_ptr_);
    for(i64 i = 0; i < 1; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), StorageException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());

        v.value_.char_n.Reset();
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }
}

TEST_F(ColumnVectorCharTest, char_column_vector_select) {
    using namespace infinity;

    auto char_info = CharInfo::Make(128);
    DataType data_type(LogicalType::kChar, char_info);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        v.value_.char_n.Reset();
    }

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for(SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++ idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++ i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(2 * i).c_str());
    }
}

TEST_F(ColumnVectorCharTest, char_column_slice_init) {
    using namespace infinity;

    auto char_info = CharInfo::Make(128);
    DataType data_type(LogicalType::kChar, char_info);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value v = Value::MakeChar(std::to_string(i), char_info);
        column_vector.AppendValue(v);
        v.value_.char_n.Reset();
    }

    for(i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++ i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(i).c_str());
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx =  3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++ i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kChar);
        EXPECT_EQ(vx.type().type_info()->type(), TypeInfoType::kChar);
        EXPECT_EQ(vx.type().type_info()->Size(), 128);

        EXPECT_STREQ(vx.value_.char_n.ptr, std::to_string(src_idx).c_str());
    }
}
