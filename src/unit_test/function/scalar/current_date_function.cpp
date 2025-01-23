// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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


#include "gtest/gtest.h"
#include <iostream>

import stl;
import base_test;
import infinity_exception;
import infinity_context;

import catalog;
import logger;

import default_values;
import value;

import base_expression;
import column_expression;
import column_vector;
import data_block;

import function_set;
import function;

import global_resource_usage;

import data_type;
import internal_types;
import logical_type;

import scalar_function;
import scalar_function_set;

import current_date;
import third_party;

using namespace infinity;

class CurrentDateFunctionsTest : public BaseTestParamStr {};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams, CurrentDateFunctionsTest, ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH));

TEST_P(CurrentDateFunctionsTest, current_date_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>();

    RegisterCurrentDateFunction(catalog_ptr);

    String op = "currentdate";

    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);
    {
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kDate);

        ScalarFunction func = scalar_function_set->GetMostMatchFunctionWithNoInput();
        EXPECT_STREQ("currentdate()->Date", func.ToString().c_str());


        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_no_(result);
        Value v = result->GetValue(0);
        EXPECT_EQ(v.type_.type(), LogicalType::kDate);
    }
}