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
import catalog;
import scalar_function;
import scalar_function_set;
import function_set;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import base_expression;
import column_vector;
import or_func;
import logical_type;
import internal_types;
import data_type;

class OrFunctionsTest : public BaseTest {};

TEST_F(OrFunctionsTest, or_func) {
    using namespace infinity;

    UniquePtr<Catalog> catalog_ptr = MakeUnique<Catalog>(MakeShared<String>("/var/infinity/data"));

    RegisterOrFunction(catalog_ptr);

    String op = "or";
    SharedPtr<FunctionSet> function_set = Catalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kBoolean);
        SharedPtr<ColumnExpression> col1_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c1", 0, 0);
        SharedPtr<ColumnExpression> col2_expr_ptr = MakeShared<ColumnExpression>(*data_type, "t1", 1, "c2", 1, 0);

        inputs.emplace_back(col1_expr_ptr);
        inputs.emplace_back(col2_expr_ptr);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("OR(Boolean, Boolean)->Boolean", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(data_type);
        column_types.emplace_back(data_type);

        SizeT row_count = DEFAULT_VECTOR_SIZE;

        DataBlock data_block;
        data_block.Init(column_types);

        for (SizeT i = 0; i < row_count; ++i) {
            if (i % 2 == 0) {
                data_block.AppendValue(0, Value::MakeBool(true));
                data_block.AppendValue(1, Value::MakeBool(false));
            } else {
                data_block.AppendValue(0, Value::MakeBool(false));
                data_block.AppendValue(1, Value::MakeBool(true));
            }
        }
        data_block.Finalize();

        for (SizeT i = 0; i < row_count; ++i) {
            Value v1 = data_block.GetValue(0, i);
            Value v2 = data_block.GetValue(1, i);
            EXPECT_EQ(v1.type_.type(), LogicalType::kBoolean);
            EXPECT_EQ(v2.type_.type(), LogicalType::kBoolean);
            if (i % 2 == 0) {
                EXPECT_EQ(v1.value_.boolean, true);
                EXPECT_EQ(v2.value_.boolean, false);
            } else {
                EXPECT_EQ(v1.value_.boolean, false);
                EXPECT_EQ(v2.value_.boolean, true);
            }
        }

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < row_count; ++i) {
            Value v = result->GetValue(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kBoolean);
            EXPECT_EQ(v.value_.boolean, true);
        }
    }
}
