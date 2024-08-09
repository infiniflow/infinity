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

class ColumnVectorUuidTest : public BaseTestParamStr {
    void SetUp() override {
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        system(("mkdir -p " + std::string(GetFullPersistDir())).c_str());
        system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
        system(("mkdir -p " + std::string(GetFullDataDir())).c_str());
        std::string config_path_str = GetParam();
        std::shared_ptr<std::string> config_path = nullptr;
        if (config_path_str != BaseTestParamStr::NULL_CONFIG_PATH) {
            config_path = infinity::MakeShared<std::string>(config_path_str);
        }
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTestParamStr::TearDown();
    }
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         ColumnVectorUuidTest,
                         ::testing::Values((std::string(infinity::test_data_path()) + "/config/test_cleanup_task_silent.toml").c_str(),
                                           BaseTestParamStr::VFS_CONFIG_PATH));

TEST_P(ColumnVectorUuidTest, flat_uuid) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kUuid);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kFlat), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), u64(DEFAULT_VECTOR_SIZE));
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
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
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
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

    EXPECT_THROW(column_vector.GetValue(0u), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kFlat);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());
        column_vector.AppendByPtr((ptr_t)(&uuid));

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }

    ColumnVector column_constant(data_type);
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        column_constant.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);
        column_constant.SetValue(0, column_vector.GetValue(i));
        column_constant.Finalize(1);

        Value vx = column_constant.GetValue(0);
        EXPECT_EQ(vx.value_.uuid, uuid);
        column_constant.Reset();
    }
}

TEST_P(ColumnVectorUuidTest, contant_uuid) {

    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kUuid);
    ColumnVector column_vector(data_type);

    column_vector.Initialize(ColumnVectorType::kConstant, DEFAULT_VECTOR_SIZE);

    
    EXPECT_THROW(column_vector.SetVectorType(ColumnVectorType::kConstant), UnrecoverableException);

    EXPECT_EQ(column_vector.capacity(), (u64)DEFAULT_VECTOR_SIZE);
    EXPECT_EQ(column_vector.Size(), 0u);

    EXPECT_THROW(column_vector.GetValue(0), UnrecoverableException);
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);

    for (i64 i = 0; i < 1; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
    for (i64 i = 0; i < 1; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
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
    EXPECT_EQ(column_vector.data_type_size_, 16u);
    EXPECT_NE(column_vector.data(), nullptr);
    EXPECT_EQ(column_vector.vector_type(), ColumnVectorType::kConstant);
    EXPECT_EQ(column_vector.data_type(), data_type);
    EXPECT_EQ(column_vector.buffer_->buffer_type_, VectorBufferType::kStandard);

    EXPECT_NE(column_vector.buffer_, nullptr);
    EXPECT_NE(column_vector.nulls_ptr_, nullptr);
    EXPECT_TRUE(column_vector.initialized);
    for (i64 i = 0; i < 1; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        column_vector.AppendValue(v);
        EXPECT_THROW(column_vector.AppendValue(v), UnrecoverableException);
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
        EXPECT_THROW(column_vector.GetValue(i + 1), UnrecoverableException);
    }
}

TEST_P(ColumnVectorUuidTest, uuid_column_vector_select) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kUuid);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
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
        String s('a' + (2 * i) % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
    }
}

TEST_P(ColumnVectorUuidTest, uuid_column_slice_init) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kUuid);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value v = Value::MakeUuid(uuid);
        column_vector.AppendValue(v);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        String s('a' + i % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
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
        String s('a' + src_idx % 26, 16);
        UuidT uuid(s.c_str());

        Value vx = target_column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kUuid);
        EXPECT_EQ(vx.value_.uuid, uuid);
    }
}
