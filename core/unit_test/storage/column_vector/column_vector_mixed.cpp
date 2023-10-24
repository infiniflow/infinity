//
// Created by JinHai on 2022/12/11.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import logger;
import column_vector;
import value;
import parser;
import default_values;
import third_party;
import stl;
import selection;
import vector_buffer;
import infinity;
import global_resource_usage;

class ColumnVectorMixedTest : public BaseTest {
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

TEST_F(ColumnVectorMixedTest, mixed_integer_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);

    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        column_vector.AppendByPtr((ptr_t)(&mixed_integer1));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_integer_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, 2 * i);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_integer_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_integer1 = MixedType::MakeInteger(i);
        Value v = Value::MakeMixedData(mixed_integer1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_integer1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, i);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kInteger);
        auto *integer_mixed_ptr = (IntegerMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(integer_mixed_ptr->value, src_idx);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_float_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        column_vector.AppendByPtr((ptr_t)(&mixed_float1));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_float_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(2 * i));
    }
}

TEST_F(ColumnVectorMixedTest, mixed_float_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_float1 = MixedType::MakeFloat(static_cast<f64>(i));
        Value v = Value::MakeMixedData(mixed_float1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_float1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(i));
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kFloat);
        auto *float_mixed_ptr = (FloatMixedType *)(&vx.value_.mixed_value);
        EXPECT_FLOAT_EQ(float_mixed_ptr->value, static_cast<f64>(src_idx));
    }
}

TEST_F(ColumnVectorMixedTest, mixed_short_str_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        column_vector.AppendByPtr((ptr_t)(&mixed_str1));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_short_str_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        String str = std::to_string(2 * i);
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
    }
}

TEST_F(ColumnVectorMixedTest, mixed_short_str_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str = std::to_string(i);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        String str = std::to_string(src_idx);
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kShortStr);
        auto *short_str_mixed_ptr = (ShortStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(short_str_mixed_ptr->length, str.size());
        EXPECT_STREQ(short_str_mixed_ptr->ptr, str.c_str());
    }
}

TEST_F(ColumnVectorMixedTest, mixed_long_str_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
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
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        column_vector.AppendByPtr((ptr_t)(&mixed_str1));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);

        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_long_str_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        String str(20, 'a' + (2 * i) % 26);
        String header(5, 'a' + (2 * i) % 26);
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }
}

TEST_F(ColumnVectorMixedTest, mixed_long_str_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        MixedT mixed_str1 = MixedType::MakeString(str);
        Value v = Value::MakeMixedData(mixed_str1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_str1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String str(20, 'a' + i % 26);
        String header(5, 'a' + i % 26);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        String str(20, 'a' + src_idx % 26);
        String header(5, 'a' + src_idx % 26);
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kLongStr);
        auto *long_str_mixed_ptr = (LongStrMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(long_str_mixed_ptr->length, str.size());

        String value_long_str(long_str_mixed_ptr->ptr, long_str_mixed_ptr->length);
        EXPECT_STREQ(value_long_str.c_str(), str.c_str());

        String value_header(long_str_mixed_ptr->header, BaseMixedType::LONG_STR_HEADER);
        EXPECT_STREQ(value_header.c_str(), header.c_str());
    }
}

TEST_F(ColumnVectorMixedTest, mixed_null_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reset();

    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_null = MixedType::MakeNull();
        column_vector.AppendByPtr((ptr_t)(&mixed_null));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_null_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_null_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_null = MixedType::MakeNull();
        Value v = Value::MakeMixedData(mixed_null);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_null);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kNull);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_missing_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reset();

    EXPECT_EQ(column_vector.capacity(), 0);
    EXPECT_EQ(column_vector.tail_index_, 0);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data_ptr_, nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        column_vector.AppendByPtr((ptr_t)(&mixed_missing));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_missing_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_missing_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedT mixed_missing = MixedType::MakeMissing();
        Value v = Value::MakeMixedData(mixed_missing);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_missing);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kMissing);
    }
}

TEST_F(ColumnVectorMixedTest, mixed_tuple_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {

        MixedType mixed_tuple1 = MixedType::MakeTuple(7);
        mixed_tuple1.InsertIntegerIntoTuple("key1", i);
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        mixed_tuple1.InsertFloatIntoTuple("key2", fvalue);

        String short_str(10, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key3", short_str);

        String long_str(20, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key4", long_str);
        mixed_tuple1.InsertNullIntoTuple("key5");

        // Key6: tuple with 5 trivial type inside
        {
            MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
            // Key61: integer
            mixed_tuple_key6.InsertIntegerIntoTuple("key61", i);
            // Key62: float
            mixed_tuple_key6.InsertFloatIntoTuple("key62", fvalue);
            // Key63: short string
            mixed_tuple_key6.InsertStringIntoTuple("key63", short_str);
            // Key64: long string
            mixed_tuple_key6.InsertStringIntoTuple("key64", long_str);
            // Key65: null
            mixed_tuple_key6.InsertNullIntoTuple("key65");

            mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
        }

        // Key7: array with 5 trivial type inside
        {
            MixedType mixed_array7 = MixedType::MakeArray(5);
            // Integer
            mixed_array7.InsertIntegerIntoArray(i, 0);
            // Float
            mixed_array7.InsertFloatIntoArray(fvalue, 1);
            // Short str
            mixed_array7.InsertStringIntoArray(short_str, 2);
            // Long str
            mixed_array7.InsertStringIntoArray(long_str, 3);
            // Null
            mixed_array7.InsertNullIntoArray(4);
            mixed_tuple1.CopyIntoTuple("key7", mixed_array7);
        }

        Value v = Value::MakeMixedData(mixed_tuple1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_tuple1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }

        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {

        MixedType mixed_tuple1 = MixedType::MakeTuple(7);
        mixed_tuple1.InsertIntegerIntoTuple("key1", i);
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        mixed_tuple1.InsertFloatIntoTuple("key2", fvalue);

        String short_str(10, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key3", short_str);

        String long_str(20, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key4", long_str);
        mixed_tuple1.InsertNullIntoTuple("key5");

        // Key6: tuple with 5 trivial type inside
        {
            MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
            // Key61: integer
            mixed_tuple_key6.InsertIntegerIntoTuple("key61", i);
            // Key62: float
            mixed_tuple_key6.InsertFloatIntoTuple("key62", fvalue);
            // Key63: short string
            mixed_tuple_key6.InsertStringIntoTuple("key63", short_str);
            // Key64: long string
            mixed_tuple_key6.InsertStringIntoTuple("key64", long_str);
            // Key65: null
            mixed_tuple_key6.InsertNullIntoTuple("key65");

            mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
        }

        // Key7: array with 5 trivial type inside
        {
            MixedType mixed_array7 = MixedType::MakeArray(5);
            // Integer
            mixed_array7.InsertIntegerIntoArray(i, 0);
            // Float
            mixed_array7.InsertFloatIntoArray(fvalue, 1);
            // Short str
            mixed_array7.InsertStringIntoArray(short_str, 2);
            // Long str
            mixed_array7.InsertStringIntoArray(long_str, 3);
            // Null
            mixed_array7.InsertNullIntoArray(4);
            mixed_tuple1.CopyIntoTuple("key7", mixed_array7);
        }

        Value v = Value::MakeMixedData(mixed_tuple1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_tuple1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }

        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
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
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {

        MixedType mixed_tuple1 = MixedType::MakeTuple(7);
        mixed_tuple1.InsertIntegerIntoTuple("key1", i);
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        mixed_tuple1.InsertFloatIntoTuple("key2", fvalue);

        String short_str(10, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key3", short_str);

        String long_str(20, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key4", long_str);
        mixed_tuple1.InsertNullIntoTuple("key5");

        // Key6: tuple with 5 trivial type inside
        {
            MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
            // Key61: integer
            mixed_tuple_key6.InsertIntegerIntoTuple("key61", i);
            // Key62: float
            mixed_tuple_key6.InsertFloatIntoTuple("key62", fvalue);
            // Key63: short string
            mixed_tuple_key6.InsertStringIntoTuple("key63", short_str);
            // Key64: long string
            mixed_tuple_key6.InsertStringIntoTuple("key64", long_str);
            // Key65: null
            mixed_tuple_key6.InsertNullIntoTuple("key65");

            mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
        }

        // Key7: array with 5 trivial type inside
        {
            MixedType mixed_array7 = MixedType::MakeArray(5);
            // Integer
            mixed_array7.InsertIntegerIntoArray(i, 0);
            // Float
            mixed_array7.InsertFloatIntoArray(fvalue, 1);
            // Short str
            mixed_array7.InsertStringIntoArray(short_str, 2);
            // Long str
            mixed_array7.InsertStringIntoArray(long_str, 3);
            // Null
            mixed_array7.InsertNullIntoArray(4);
            mixed_tuple1.CopyIntoTuple("key7", mixed_array7);
        }

        column_vector.AppendByPtr((ptr_t)(&mixed_tuple1));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }

        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedType mixed_tuple1 = MixedType::MakeTuple(7);
        mixed_tuple1.InsertIntegerIntoTuple("key1", i);
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        mixed_tuple1.InsertFloatIntoTuple("key2", fvalue);

        String short_str(10, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key3", short_str);

        String long_str(20, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key4", long_str);
        mixed_tuple1.InsertNullIntoTuple("key5");

        // Key6: tuple with 5 trivial type inside
        {
            MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
            // Key61: integer
            mixed_tuple_key6.InsertIntegerIntoTuple("key61", i);
            // Key62: float
            mixed_tuple_key6.InsertFloatIntoTuple("key62", fvalue);
            // Key63: short string
            mixed_tuple_key6.InsertStringIntoTuple("key63", short_str);
            // Key64: long string
            mixed_tuple_key6.InsertStringIntoTuple("key64", long_str);
            // Key65: null
            mixed_tuple_key6.InsertNullIntoTuple("key65");

            mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
        }

        // Key7: array with 5 trivial type inside
        {
            MixedType mixed_array7 = MixedType::MakeArray(5);
            // Integer
            mixed_array7.InsertIntegerIntoArray(i, 0);
            // Float
            mixed_array7.InsertFloatIntoArray(fvalue, 1);
            // Short str
            mixed_array7.InsertStringIntoArray(short_str, 2);
            // Long str
            mixed_array7.InsertStringIntoArray(long_str, 3);
            // Null
            mixed_array7.InsertNullIntoArray(4);
            mixed_tuple1.CopyIntoTuple("key7", mixed_array7);
        }

        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_tuple_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedType mixed_tuple1 = MixedType::MakeTuple(7);
        mixed_tuple1.InsertIntegerIntoTuple("key1", i);
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        mixed_tuple1.InsertFloatIntoTuple("key2", fvalue);

        String short_str(10, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key3", short_str);

        String long_str(20, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key4", long_str);
        mixed_tuple1.InsertNullIntoTuple("key5");

        // Key6: tuple with 5 trivial type inside
        {
            MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
            // Key61: integer
            mixed_tuple_key6.InsertIntegerIntoTuple("key61", i);
            // Key62: float
            mixed_tuple_key6.InsertFloatIntoTuple("key62", fvalue);
            // Key63: short string
            mixed_tuple_key6.InsertStringIntoTuple("key63", short_str);
            // Key64: long string
            mixed_tuple_key6.InsertStringIntoTuple("key64", long_str);
            // Key65: null
            mixed_tuple_key6.InsertNullIntoTuple("key65");

            mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
        }

        // Key7: array with 5 trivial type inside
        {
            MixedType mixed_array7 = MixedType::MakeArray(5);
            // Integer
            mixed_array7.InsertIntegerIntoArray(i, 0);
            // Float
            mixed_array7.InsertFloatIntoArray(fvalue, 1);
            // Short str
            mixed_array7.InsertStringIntoArray(short_str, 2);
            // Long str
            mixed_array7.InsertStringIntoArray(long_str, 3);
            // Null
            mixed_array7.InsertNullIntoArray(4);
            mixed_tuple1.CopyIntoTuple("key7", mixed_array7);
        }

        Value v = Value::MakeMixedData(mixed_tuple1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_tuple1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        f64 fvalue = static_cast<f64>(2 * i) + static_cast<f64>(2 * i) / 10.0f;
        String short_str(10, 'a' + (2 * i) % 26);
        String long_str(20, 'a' + (2 * i) % 26);

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, 2 * i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, 2 * i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 2 * i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }
    }
}

TEST_F(ColumnVectorMixedTest, mixed_tuple_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        MixedType mixed_tuple1 = MixedType::MakeTuple(7);
        mixed_tuple1.InsertIntegerIntoTuple("key1", i);
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        mixed_tuple1.InsertFloatIntoTuple("key2", fvalue);

        String short_str(10, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key3", short_str);

        String long_str(20, 'a' + i % 26);
        mixed_tuple1.InsertStringIntoTuple("key4", long_str);
        mixed_tuple1.InsertNullIntoTuple("key5");

        // Key6: tuple with 5 trivial type inside
        {
            MixedType mixed_tuple_key6 = MixedType::MakeTuple(5);
            // Key61: integer
            mixed_tuple_key6.InsertIntegerIntoTuple("key61", i);
            // Key62: float
            mixed_tuple_key6.InsertFloatIntoTuple("key62", fvalue);
            // Key63: short string
            mixed_tuple_key6.InsertStringIntoTuple("key63", short_str);
            // Key64: long string
            mixed_tuple_key6.InsertStringIntoTuple("key64", long_str);
            // Key65: null
            mixed_tuple_key6.InsertNullIntoTuple("key65");

            mixed_tuple1.CopyIntoTuple("key6", mixed_tuple_key6);
        }

        // Key7: array with 5 trivial type inside
        {
            MixedType mixed_array7 = MixedType::MakeArray(5);
            // Integer
            mixed_array7.InsertIntegerIntoArray(i, 0);
            // Float
            mixed_array7.InsertFloatIntoArray(fvalue, 1);
            // Short str
            mixed_array7.InsertStringIntoArray(short_str, 2);
            // Long str
            mixed_array7.InsertStringIntoArray(long_str, 3);
            // Null
            mixed_array7.InsertNullIntoArray(4);
            mixed_tuple1.CopyIntoTuple("key7", mixed_array7);
        }

        Value v = Value::MakeMixedData(mixed_tuple1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_tuple1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, i);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, i);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        f64 fvalue = static_cast<f64>(src_idx) + static_cast<f64>(src_idx) / 10.0f;
        String short_str(10, 'a' + src_idx % 26);
        String long_str(20, 'a' + src_idx % 26);

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kTuple);

        const MixedT &tuple_ref = vx.value_.mixed_value;
        auto *tuple_mixed_ptr = (TupleMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(tuple_mixed_ptr->count, 7);
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key1");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
            auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
            EXPECT_EQ(integer_value_ptr->value, src_idx);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key2");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kFloat);
            auto *float_value_ptr = (FloatMixedType *)(value1_ptr);
            EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key3");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kShortStr);
            auto *short_str_value_ptr = (ShortStrMixedType *)(value1_ptr);

            String result(short_str_value_ptr->ptr, short_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), short_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key4");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kLongStr);
            auto *long_str_value_ptr = (LongStrMixedType *)(value1_ptr);

            String result(long_str_value_ptr->ptr, long_str_value_ptr->length);
            EXPECT_STREQ(result.c_str(), long_str.c_str());
        }
        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key5");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kNull);
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key6");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kTuple);
            auto *tuple_value_ptr = (TupleMixedType *)(value1_ptr);
            EXPECT_EQ(tuple_value_ptr->count, 5);
            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key61");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kInteger);
                auto *nested_integer_value_ptr = (IntegerMixedType *)(nested_value_ptr);
                EXPECT_EQ(nested_integer_value_ptr->value, src_idx);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key62");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kFloat);
                auto *nested_float_value_ptr = (FloatMixedType *)(nested_value_ptr);
                EXPECT_FLOAT_EQ(nested_float_value_ptr->value, fvalue);
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key63");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kShortStr);
                auto *nested_short_str_ptr = (ShortStrMixedType *)(nested_value_ptr);

                String result(nested_short_str_ptr->ptr, nested_short_str_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key64");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kLongStr);
                auto *nested_long_str_ptr = (LongStrMixedType *)(nested_value_ptr);

                String result(nested_long_str_ptr->ptr, nested_long_str_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }

            {
                auto *nested_value_ptr = value1_ptr->GetFromTuple("key65");
                EXPECT_EQ(nested_value_ptr->type, MixedValueType::kNull);
            }
        }

        {
            auto *value1_ptr = tuple_ref.GetFromTuple("key7");
            EXPECT_EQ(value1_ptr->type, MixedValueType::kArray);
            auto *array_value_ptr = (ArrayMixedType *)(value1_ptr);
            EXPECT_EQ(array_value_ptr->count, 5);

            {
                auto *array_value1_ptr = value1_ptr->GetByIndex(0);
                EXPECT_EQ(array_value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(array_value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, src_idx);
            }
            {
                auto *array_value2_ptr = value1_ptr->GetByIndex(1);
                EXPECT_EQ(array_value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(array_value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            {
                auto *array_value3_ptr = value1_ptr->GetByIndex(2);
                EXPECT_EQ(array_value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(array_value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // Long str
            {
                auto *array_value4_ptr = value1_ptr->GetByIndex(3);
                EXPECT_EQ(array_value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(array_value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // Null
            {
                auto *array_value5_ptr = value1_ptr->GetByIndex(4);
                EXPECT_EQ(array_value5_ptr->type, MixedValueType::kNull);
            }
        }
    }
}

TEST_F(ColumnVectorMixedTest, mixed_array_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        MixedType mixed_array1 = MixedType::MakeArray(7);
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        // 1. Integer
        mixed_array1.InsertIntegerIntoArray(i, 0);
        // 2. Float
        mixed_array1.InsertFloatIntoArray(fvalue, 1);
        // 3. Short str
        mixed_array1.InsertStringIntoArray(short_str, 2);
        // 4. Long str
        mixed_array1.InsertStringIntoArray(long_str, 3);
        // 5. Null
        mixed_array1.InsertNullIntoArray(4);
        // 6. Tuple
        {
            MixedType mixed_tuple5 = MixedType::MakeTuple(5);
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            // Key51: integer
            mixed_tuple5.InsertIntegerIntoTuple("key51", i);
            // Key52: float
            mixed_tuple5.InsertFloatIntoTuple("key52", fvalue);
            // Key53: short string
            mixed_tuple5.InsertStringIntoTuple("key53", short_str);
            // Key54: long string
            mixed_tuple5.InsertStringIntoTuple("key54", long_str);
            // Key55: null
            mixed_tuple5.InsertNullIntoTuple("key55");

            mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        }

        {
            MixedType nested_array = MixedType::MakeArray(5);
            // Integer
            nested_array.InsertIntegerIntoArray(i, 0);
            // Float
            nested_array.InsertFloatIntoArray(fvalue, 1);
            // Short str
            nested_array.InsertStringIntoArray(short_str, 2);
            // Long str
            nested_array.InsertStringIntoArray(long_str, 3);
            // Null
            nested_array.InsertNullIntoArray(4);

            mixed_array1.CopyIntoArray(nested_array, 6);
        }

        Value v = Value::MakeMixedData(mixed_array1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_array1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = mixed_array1.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }

        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    column_vector.Reserve(DEFAULT_VECTOR_SIZE * 2);

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

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = array_ref.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }
    }

    EXPECT_EQ(column_vector.tail_index_, DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.capacity(), 2 * DEFAULT_VECTOR_SIZE);
    for (i64 i = DEFAULT_VECTOR_SIZE; i < 2 * DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        MixedType mixed_array1 = MixedType::MakeArray(7);
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        // 1. Integer
        mixed_array1.InsertIntegerIntoArray(i, 0);
        // 2. Float
        mixed_array1.InsertFloatIntoArray(fvalue, 1);
        // 3. Short str
        mixed_array1.InsertStringIntoArray(short_str, 2);
        // 4. Long str
        mixed_array1.InsertStringIntoArray(long_str, 3);
        // 5. Null
        mixed_array1.InsertNullIntoArray(4);
        // 6. Tuple
        {
            MixedType mixed_tuple5 = MixedType::MakeTuple(5);
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            // Key51: integer
            mixed_tuple5.InsertIntegerIntoTuple("key51", i);
            // Key52: float
            mixed_tuple5.InsertFloatIntoTuple("key52", fvalue);
            // Key53: short string
            mixed_tuple5.InsertStringIntoTuple("key53", short_str);
            // Key54: long string
            mixed_tuple5.InsertStringIntoTuple("key54", long_str);
            // Key55: null
            mixed_tuple5.InsertNullIntoTuple("key55");

            mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        }

        {
            MixedType nested_array = MixedType::MakeArray(5);
            // Integer
            nested_array.InsertIntegerIntoArray(i, 0);
            // Float
            nested_array.InsertFloatIntoArray(fvalue, 1);
            // Short str
            nested_array.InsertStringIntoArray(short_str, 2);
            // Long str
            nested_array.InsertStringIntoArray(long_str, 3);
            // Null
            nested_array.InsertNullIntoArray(4);

            mixed_array1.CopyIntoArray(nested_array, 6);
        }

        Value v = Value::MakeMixedData(mixed_array1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_array1);

        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = mixed_array1.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }

        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
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
    column_vector.Initialize();
    EXPECT_THROW(column_vector.SetDataType(data_type), TypeException);
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), TypeException);

    EXPECT_EQ(column_vector.capacity(), DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0);

    EXPECT_THROW(column_vector.GetValue(0), TypeException);
    EXPECT_EQ(column_vector.tail_index_, 0);
    EXPECT_EQ(column_vector.data_type_size_, 16);
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
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        MixedType mixed_array1 = MixedType::MakeArray(7);
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        // 1. Integer
        mixed_array1.InsertIntegerIntoArray(i, 0);
        // 2. Float
        mixed_array1.InsertFloatIntoArray(fvalue, 1);
        // 3. Short str
        mixed_array1.InsertStringIntoArray(short_str, 2);
        // 4. Long str
        mixed_array1.InsertStringIntoArray(long_str, 3);
        // 5. Null
        mixed_array1.InsertNullIntoArray(4);
        // 6. Tuple
        {
            MixedType mixed_tuple5 = MixedType::MakeTuple(5);
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            // Key51: integer
            mixed_tuple5.InsertIntegerIntoTuple("key51", i);
            // Key52: float
            mixed_tuple5.InsertFloatIntoTuple("key52", fvalue);
            // Key53: short string
            mixed_tuple5.InsertStringIntoTuple("key53", short_str);
            // Key54: long string
            mixed_tuple5.InsertStringIntoTuple("key54", long_str);
            // Key55: null
            mixed_tuple5.InsertNullIntoTuple("key55");

            mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        }

        {
            MixedType nested_array = MixedType::MakeArray(5);
            // Integer
            nested_array.InsertIntegerIntoArray(i, 0);
            // Float
            nested_array.InsertFloatIntoArray(fvalue, 1);
            // Short str
            nested_array.InsertStringIntoArray(short_str, 2);
            // Long str
            nested_array.InsertStringIntoArray(long_str, 3);
            // Null
            nested_array.InsertNullIntoArray(4);

            mixed_array1.CopyIntoArray(nested_array, 6);
        }

        column_vector.AppendByPtr((ptr_t)(&mixed_array1));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = mixed_array1.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }

        EXPECT_THROW(column_vector.GetValue(i + 1), TypeException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        MixedType mixed_array1 = MixedType::MakeArray(7);
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        // 1. Integer
        mixed_array1.InsertIntegerIntoArray(i, 0);
        // 2. Float
        mixed_array1.InsertFloatIntoArray(fvalue, 1);
        // 3. Short str
        mixed_array1.InsertStringIntoArray(short_str, 2);
        // 4. Long str
        mixed_array1.InsertStringIntoArray(long_str, 3);
        // 5. Null
        mixed_array1.InsertNullIntoArray(4);
        // 6. Tuple
        {
            MixedType mixed_tuple5 = MixedType::MakeTuple(5);
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            // Key51: integer
            mixed_tuple5.InsertIntegerIntoTuple("key51", i);
            // Key52: float
            mixed_tuple5.InsertFloatIntoTuple("key52", fvalue);
            // Key53: short string
            mixed_tuple5.InsertStringIntoTuple("key53", short_str);
            // Key54: long string
            mixed_tuple5.InsertStringIntoTuple("key54", long_str);
            // Key55: null
            mixed_tuple5.InsertNullIntoTuple("key55");

            mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        }

        {
            MixedType nested_array = MixedType::MakeArray(5);
            // Integer
            nested_array.InsertIntegerIntoArray(i, 0);
            // Float
            nested_array.InsertFloatIntoArray(fvalue, 1);
            // Short str
            nested_array.InsertStringIntoArray(short_str, 2);
            // Long str
            nested_array.InsertStringIntoArray(long_str, 3);
            // Null
            nested_array.InsertNullIntoArray(4);

            mixed_array1.CopyIntoArray(nested_array, 6);
        }

        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.CopyRow(column_vector, 0, i);
        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = mixed_array1.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorMixedTest, mixed_array_column_vector_select) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        MixedType mixed_array1 = MixedType::MakeArray(7);
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        // 1. Integer
        mixed_array1.InsertIntegerIntoArray(i, 0);
        // 2. Float
        mixed_array1.InsertFloatIntoArray(fvalue, 1);
        // 3. Short str
        mixed_array1.InsertStringIntoArray(short_str, 2);
        // 4. Long str
        mixed_array1.InsertStringIntoArray(long_str, 3);
        // 5. Null
        mixed_array1.InsertNullIntoArray(4);
        // 6. Tuple
        {
            MixedType mixed_tuple5 = MixedType::MakeTuple(5);
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            // Key51: integer
            mixed_tuple5.InsertIntegerIntoTuple("key51", i);
            // Key52: float
            mixed_tuple5.InsertFloatIntoTuple("key52", fvalue);
            // Key53: short string
            mixed_tuple5.InsertStringIntoTuple("key53", short_str);
            // Key54: long string
            mixed_tuple5.InsertStringIntoTuple("key54", long_str);
            // Key55: null
            mixed_tuple5.InsertNullIntoTuple("key55");

            mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        }

        {
            MixedType nested_array = MixedType::MakeArray(5);
            // Integer
            nested_array.InsertIntegerIntoArray(i, 0);
            // Float
            nested_array.InsertFloatIntoArray(fvalue, 1);
            // Short str
            nested_array.InsertStringIntoArray(short_str, 2);
            // Long str
            nested_array.InsertStringIntoArray(long_str, 3);
            // Null
            nested_array.InsertNullIntoArray(4);

            mixed_array1.CopyIntoArray(nested_array, 6);
        }

        Value v = Value::MakeMixedData(mixed_array1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_array1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = array_ref.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        f64 fvalue = static_cast<f64>(2 * i) + static_cast<f64>(2 * i) / 10.0f;
        String short_str(10, 'a' + (2 * i) % 26);
        String long_str(20, 'a' + (2 * i) % 26);

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, 2 * i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = array_ref.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, 2 * i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, 2 * i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }
    }
}

TEST_F(ColumnVectorMixedTest, mixed_array_column_slice_init) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kMixed);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        MixedType mixed_array1 = MixedType::MakeArray(7);
        EXPECT_EQ(mixed_array1.type, MixedValueType::kArray);
        // 1. Integer
        mixed_array1.InsertIntegerIntoArray(i, 0);
        // 2. Float
        mixed_array1.InsertFloatIntoArray(fvalue, 1);
        // 3. Short str
        mixed_array1.InsertStringIntoArray(short_str, 2);
        // 4. Long str
        mixed_array1.InsertStringIntoArray(long_str, 3);
        // 5. Null
        mixed_array1.InsertNullIntoArray(4);
        // 6. Tuple
        {
            MixedType mixed_tuple5 = MixedType::MakeTuple(5);
            EXPECT_EQ(mixed_tuple5.type, MixedValueType::kTuple);
            // Key51: integer
            mixed_tuple5.InsertIntegerIntoTuple("key51", i);
            // Key52: float
            mixed_tuple5.InsertFloatIntoTuple("key52", fvalue);
            // Key53: short string
            mixed_tuple5.InsertStringIntoTuple("key53", short_str);
            // Key54: long string
            mixed_tuple5.InsertStringIntoTuple("key54", long_str);
            // Key55: null
            mixed_tuple5.InsertNullIntoTuple("key55");

            mixed_array1.CopyIntoArray(mixed_tuple5, 5);
        }

        {
            MixedType nested_array = MixedType::MakeArray(5);
            // Integer
            nested_array.InsertIntegerIntoArray(i, 0);
            // Float
            nested_array.InsertFloatIntoArray(fvalue, 1);
            // Short str
            nested_array.InsertStringIntoArray(short_str, 2);
            // Long str
            nested_array.InsertStringIntoArray(long_str, 3);
            // Null
            nested_array.InsertNullIntoArray(4);

            mixed_array1.CopyIntoArray(nested_array, 6);
        }

        Value v = Value::MakeMixedData(mixed_array1);
        EXPECT_EQ(v.GetValue<MixedT>(), mixed_array1);

        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        f64 fvalue = static_cast<f64>(i) + static_cast<f64>(i) / 10.0f;
        String short_str(10, 'a' + i % 26);
        String long_str(20, 'a' + i % 26);

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, i);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = array_ref.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, i);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        f64 fvalue = static_cast<f64>(src_idx) + static_cast<f64>(src_idx) / 10.0f;
        String short_str(10, 'a' + src_idx % 26);
        String long_str(20, 'a' + src_idx % 26);

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kMixed);
        EXPECT_EQ(vx.value_.mixed_value.type, MixedValueType::kArray);

        const MixedT &array_ref = vx.value_.mixed_value;
        auto *array_mixed_ptr = (ArrayMixedType *)(&vx.value_.mixed_value);
        EXPECT_EQ(array_mixed_ptr->count, 7);

        {
            // 1. Integer
            {
                auto *value1_ptr = array_ref.GetByIndex(0);
                EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                EXPECT_EQ(integer_value_ptr->value, src_idx);
            }
            // 2. Float
            {
                auto *value2_ptr = array_ref.GetByIndex(1);
                EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
            }
            // 3. Short str
            {
                auto *value3_ptr = array_ref.GetByIndex(2);
                EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), short_str.c_str());
            }
            // 4. Long str
            {
                auto *value4_ptr = array_ref.GetByIndex(3);
                EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                EXPECT_STREQ(result.c_str(), long_str.c_str());
            }
            // 5. Null
            {
                auto *value5_ptr = array_ref.GetByIndex(4);
                EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
            }

            // 6. Tuple
            {
                auto *value6_ptr = array_ref.GetByIndex(5);
                {
                    EXPECT_EQ(value6_ptr->type, MixedValueType::kTuple);
                    {
                        auto *tuple_ptr5 = (TupleMixedType *)(value6_ptr);
                        EXPECT_EQ(tuple_ptr5->count, 5);
                    }
                    // Key51: integer
                    {
                        auto *value1_ptr = value6_ptr->GetFromTuple("key51");
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, src_idx);
                    }
                    // Key52: float
                    {
                        auto *value2_ptr = value6_ptr->GetFromTuple("key52");
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Key53: short string
                    {
                        auto *value3_ptr = value6_ptr->GetFromTuple("key53");
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Key54: long string
                    {
                        auto *value4_ptr = value6_ptr->GetFromTuple("key54");
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Key55: null
                    {
                        auto *value5_ptr = value6_ptr->GetFromTuple("key55");
                        EXPECT_EQ(value5_ptr->type, MixedValueType::kNull);
                    }
                }
            }

            // 7. Array
            {
                auto *value7_ptr = array_ref.GetByIndex(6);
                {
                    EXPECT_EQ(value7_ptr->type, MixedValueType::kArray);
                    {
                        auto *nested_array_ptr1 = (ArrayMixedType *)(value7_ptr);
                        EXPECT_EQ(nested_array_ptr1->count, 5);
                    }
                    // Integer
                    {
                        auto *value1_ptr = value7_ptr->GetByIndex(0);
                        EXPECT_EQ(value1_ptr->type, MixedValueType::kInteger);
                        auto *integer_value_ptr = (IntegerMixedType *)(value1_ptr);
                        EXPECT_EQ(integer_value_ptr->value, src_idx);
                    }
                    // Float
                    {
                        auto *value2_ptr = value7_ptr->GetByIndex(1);
                        EXPECT_EQ(value2_ptr->type, MixedValueType::kFloat);
                        auto *float_value_ptr = (FloatMixedType *)(value2_ptr);
                        EXPECT_FLOAT_EQ(float_value_ptr->value, fvalue);
                    }
                    // Short str
                    {
                        auto *value3_ptr = value7_ptr->GetByIndex(2);
                        EXPECT_EQ(value3_ptr->type, MixedValueType::kShortStr);
                        auto *short_str_value_ptr = (ShortStrMixedType *)(value3_ptr);

                        String result = String(short_str_value_ptr->ptr, short_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), short_str.c_str());
                    }
                    // Long str
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(3);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kLongStr);
                        auto *long_str_value_ptr = (LongStrMixedType *)(value4_ptr);

                        String result = String(long_str_value_ptr->ptr, long_str_value_ptr->length);
                        EXPECT_STREQ(result.c_str(), long_str.c_str());
                    }

                    // Null
                    {
                        auto *value4_ptr = value7_ptr->GetByIndex(4);
                        EXPECT_EQ(value4_ptr->type, MixedValueType::kNull);
                    }
                }
            }
        }
    }
}