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

#include "unit_test/base_test.h"

import infinity_exception;

import logger;
import column_vector;
import value;

import default_values;
import third_party;
import stl;
import selection;
import vector_buffer;
import global_resource_usage;
import infinity_context;
import internal_types;
import logical_type;
import data_type;
import compilation_config;

class ColumnVectorFloatTest : public BaseTestParamStr {
    void SetUp() override {
        using namespace infinity;

        LoggerConfig logger_config;
        logger_config.log_level_ = LogLevel::kOff;
        Logger::Initialize(logger_config);
//         RemoveDbDirs();
// #ifdef INFINITY_DEBUG
//         infinity::GlobalResourceUsage::Init();
// #endif
//         system(("mkdir -p " + std::string(GetFullPersistDir())).c_str());
//         system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
//         system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
//         std::string config_path_str = GetParam();
//         std::shared_ptr<std::string> config_path = nullptr;
//         if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
//             config_path = infinity::MakeShared<std::string>(config_path_str);
//         }
//         infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        using namespace infinity;

        Logger::Shutdown();
//         infinity::InfinityContext::instance().UnInit();
// #ifdef INFINITY_DEBUG
//         EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
//         EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
//         infinity::GlobalResourceUsage::UnInit();
// #endif
//         BaseTestParamStr::TearDown();
    }
};

// INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
//                          ColumnVectorFloatTest,
//                          ::testing::Values((std::string(infinity::test_data_path()) + "/config/test_cleanup_task_silent.toml").c_str(),
//                                            BaseTestParamStr::VFS_CONFIG_PATH));

TEST_F(ColumnVectorFloatTest, flat_float) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), u64(DEFAULT_VECTOR_SIZE));
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 4u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);
    //    EXPECT_EQ(column_vector.data_type(), DataType(LogicalType::kInvalid));
    //    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kInvalid);

    // ====
    //    EXPECT_THROW(column_vector.Initialize(), UnrecoverableException);
    //    EXPECT_THROW(column_vector.Initialize(), UnrecoverableException);
    //    column_vector.SetVectorType(ColumnVectorType::kFlat);
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 4u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        FloatT f = static_cast<FloatT>(i) + 0.5f;
        column_vector.AppendByPtr((ptr_t)(&f));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorFloatTest, contant_float) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 4u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 4u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_F(ColumnVectorFloatTest, float_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(2 * i) + 0.5f);
    }
}

TEST_F(ColumnVectorFloatTest, float_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kFloat);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(i) + 0.5f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kFloat);
        EXPECT_FLOAT_EQ(vx.value_.float32, static_cast<FloatT>(src_idx) + 0.5f);
    }
}

TEST_F(ColumnVectorFloatTest, flat_double) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector clone_column_vector(data_type);
    clone_column_vector.ShallowCopy(column_vector);
    EXPECT_EQ(column_vector.Size(), clone_column_vector.Size());
    EXPECT_EQ(column_vector.capacity(), clone_column_vector.capacity());
    EXPECT_EQ(column_vector.data_type(), clone_column_vector.data_type());
    EXPECT_EQ(column_vector.data(), clone_column_vector.data());
    EXPECT_EQ(column_vector.data_type_size_, clone_column_vector.data_type_size_);
    EXPECT_EQ(column_vector.nulls_ptr_, clone_column_vector.nulls_ptr_);
    EXPECT_EQ(column_vector.buffer_, clone_column_vector.buffer_);
    EXPECT_EQ(column_vector.initialized, clone_column_vector.initialized);
    EXPECT_EQ(column_vector.vector_type(), clone_column_vector.vector_type());

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize();
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        DoubleT f = static_cast<DoubleT>(i) + 0.8f;
        column_vector.AppendByPtr((ptr_t)(&f));
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        column_constant.Reset();
    }
}

TEST_F(ColumnVectorFloatTest, contant_double) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
    }

    column_vector.Reset();
    EXPECT_EQ(column_vector.capacity(), 0u);
    EXPECT_EQ(column_vector.Size(), 0u);
    //    EXPECT_EQ(column_vector.data_type_size_, 0);
    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_EQ(column_vector.buffer_->fix_heap_mgr_, nullptr);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.initialized, false);

    // ====
    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 8u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_FLOAT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_F(ColumnVectorFloatTest, double_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
    }

    Selection input_select;
    input_select.Initialize(DEFAULT_VECTOR_SIZE / 2);
    for (SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++idx) {
        input_select.Append(idx * 2);
    }

    ColumnVector target_column_vector(data_type);
    target_column_vector.Initialize(column_vector, input_select);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE / 2; ++i) {
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(2 * i) + 0.8f);
    }
}

TEST_F(ColumnVectorFloatTest, double_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kDouble);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeDouble(static_cast<DoubleT>(i) + 0.8f);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(i) + 0.8f);
    }

    ColumnVector target_column_vector(data_type);
    i64 start_idx = DEFAULT_VECTOR_SIZE / 4;
    i64 end_idx = 3 * DEFAULT_VECTOR_SIZE / 4;
    i64 count = end_idx - start_idx;
    target_column_vector.Initialize(column_vector, start_idx, end_idx);
    EXPECT_EQ(target_column_vector.Size(), (u64)DEFAULT_VECTOR_SIZE / 2);
    EXPECT_EQ(count, DEFAULT_VECTOR_SIZE / 2);

    for (i64 i = 0; i < count; ++i) {
        i64 src_idx = start_idx + i;
        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kDouble);
        EXPECT_EQ(vx.value_.float64, static_cast<DoubleT>(src_idx) + 0.8f);
    }
}
