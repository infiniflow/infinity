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

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
import default_values;
import data_table;
import table_def;
import value;
import data_block;
import column_vector;
import logger;
import selection;
import logical_type;
import internal_types;
import data_type;

class ExpressionExecutorSelectTest : public BaseTest {
    void SetUp() override {
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }
};

TEST_F(ExpressionExecutorSelectTest, test1) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(static_cast<BooleanT>(i % 2 == 0));
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
    }

    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionSelector::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    //    EXPECT_EQ(output_true_select->Size(), DEFAULT_VECTOR_SIZE / 2);
    //    for(SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++ idx) {
    //        EXPECT_EQ((*output_true_select)[idx], idx * 2);
    //    }
}

TEST_F(ExpressionExecutorSelectTest, test2) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(true);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, true);
    }

    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionExecutor::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    //    EXPECT_EQ(output_true_select->Size(), DEFAULT_VECTOR_SIZE);
    //    for(SizeT idx = 0; idx < DEFAULT_VECTOR_SIZE; ++ idx) {
    //        EXPECT_EQ((*output_true_select)[idx], idx);
    //    }
}

TEST_F(ExpressionExecutorSelectTest, test3) {
    using namespace infinity;

    SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(false);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValue(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, false);
    }

    SharedPtr<Selection> output_true_select = MakeShared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionExecutor::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    EXPECT_EQ(output_true_select->Size(), 0u);
    EXPECT_THROW((*output_true_select)[0], UnrecoverableException);
}
