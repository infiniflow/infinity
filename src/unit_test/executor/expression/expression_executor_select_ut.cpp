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

#ifdef CI
#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"

module infinity_core:ut.expression_executor_select;

import :ut.base_test;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
import :default_values;
import :value;
import :logger;
import :selection;
#endif

import global_resource_usage;
import logical_type;
import internal_types;
import data_type;

using namespace infinity;
class ExpressionExecutorSelectTest : public BaseTest {};

TEST_F(ExpressionExecutorSelectTest, test1) {
    using namespace infinity;

    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(static_cast<BooleanT>(i % 2 == 0));
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValueByIndex(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, static_cast<BooleanT>(i % 2 == 0));
    }

    std::shared_ptr<Selection> output_true_select = std::make_shared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionSelector::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    //    EXPECT_EQ(output_true_select->Size(), DEFAULT_VECTOR_SIZE / 2);
    //    for(size_t idx = 0; idx < DEFAULT_VECTOR_SIZE / 2; ++ idx) {
    //        EXPECT_EQ((*output_true_select)[idx], idx * 2);
    //    }
}

TEST_F(ExpressionExecutorSelectTest, test2) {
    using namespace infinity;

    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(true);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValueByIndex(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, true);
    }

    std::shared_ptr<Selection> output_true_select = std::make_shared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionExecutor::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    //    EXPECT_EQ(output_true_select->Size(), DEFAULT_VECTOR_SIZE);
    //    for(size_t idx = 0; idx < DEFAULT_VECTOR_SIZE; ++ idx) {
    //        EXPECT_EQ((*output_true_select)[idx], idx);
    //    }
}

TEST_F(ExpressionExecutorSelectTest, test3) {
    using namespace infinity;

    std::shared_ptr<DataType> data_type = std::make_shared<DataType>(LogicalType::kBoolean);
    ColumnVector column_vector(data_type);
    column_vector.Initialize();

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value v = Value::MakeBool(false);
        column_vector.AppendValue(v);
        Value vx = column_vector.GetValueByIndex(i);
    }

    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        Value vx = column_vector.GetValueByIndex(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBoolean);
        EXPECT_EQ(vx.value_.boolean, false);
    }

    std::shared_ptr<Selection> output_true_select = std::make_shared<Selection>();
    output_true_select->Initialize(DEFAULT_VECTOR_SIZE);
#if 0
    ExpressionExecutor::Select((const u8 *)(column_vector.data()),
                               column_vector.nulls_ptr_,
                               DEFAULT_VECTOR_SIZE,
                               output_true_select,
                               true);
#endif
    EXPECT_EQ(output_true_select->Size(), 0u);
    EXPECT_THROW_WITHOUT_STACKTRACE((*output_true_select)[0], UnrecoverableException);
}
