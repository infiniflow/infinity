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
import infinity_exception;
import column_vector;

import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import varchar_cast;
import internal_types;
import logical_type;
import data_type;
import logical_type;

class VarcharTest : public BaseTest {};

using namespace infinity;

void InitVarcharAsInline(VarcharT& varchar, const std::string_view sv) {
    varchar.is_value_ = false;
    varchar.length_ = sv.size();
    if (sv.size() <= VARCHAR_INLINE_LEN) {
        std::memcpy(varchar.short_.data_, sv.data(), sv.size());
    } else {
        UnrecoverableError("Varchar length is too long");
    }
}

TEST_F(VarcharTest, varchar_cast0) {
    {

        DataType source_type(LogicalType::kVarchar);
        String s1 = "true";

        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{false};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(target);
            EXPECT_TRUE(result);
        }

        s1 = "false";
        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{true};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(target);
            EXPECT_TRUE(result);
        }

        s1 = "falsk";
        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{true};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
        }
    }

    {

        DataType source_type(LogicalType::kVarchar);
        String s1 = "-128";

        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
            EXPECT_EQ(target, -128);
        }

        s1 = "127";
        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
            EXPECT_EQ(target, 127);
        }

        s1 = "190";
        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
        }

        s1 = "abc";
        {
            VarcharT v1;
            InitVarcharAsInline(v1, s1);
            EXPECT_EQ(v1.length_, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
            EXPECT_EQ(target, 0);
        }
    }
}

TEST_F(VarcharTest, varchar_cast1) {
    using namespace infinity;


    {
        SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> input_column_vector = MakeShared<ColumnVector>(source_type);
        input_column_vector->Initialize();

        // Case 1
        String s1 = "100";
        Value v = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v);

        VarcharT* varchar_ptr = (VarcharT*)(input_column_vector->buffer_.get()->GetDataMut());

        BigIntT target{0};
        bool result = TryCastVarcharVector::Run(varchar_ptr[0], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_EQ(target, 100);

        // Case 2
        s1 = "-100";

        Value v1 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v1);

        result = TryCastVarcharVector::Run(varchar_ptr[1], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_EQ(target, -100);

        // Case 3
        s1 = "1000000000000000";

        Value v2 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v2);

        result = TryCastVarcharVector::Run(varchar_ptr[2], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_EQ(target, 1000000000000000);

        // Case 4
        s1 = "-1000000000000000";

        Value v3 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v3);

        result = TryCastVarcharVector::Run(varchar_ptr[3], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_EQ(target, -1000000000000000);
    }

    {
        SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> input_column_vector = MakeShared<ColumnVector>(source_type);
        input_column_vector->Initialize();

        // Case 1
        String s1 = "100.01";
        Value v = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v);

        VarcharT* varchar_ptr = (VarcharT*)(input_column_vector->buffer_.get()->GetDataMut());

        FloatT target{0};
        bool result = TryCastVarcharVector::Run(varchar_ptr[0], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, 100.01);

        // Case 2
        s1 = "-100.02";

        Value v1 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v1);

        result = TryCastVarcharVector::Run(varchar_ptr[1], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, -100.02);

        // Case 3
        s1 = "100000.345";

        Value v2 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v2);

        result = TryCastVarcharVector::Run(varchar_ptr[2], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, 100000.345);

        // Case 4
        s1 = "-1000000000.4565544";

        Value v3 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v3);

        result = TryCastVarcharVector::Run(varchar_ptr[3], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, -1000000000.4565544);
    }


    {
        SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> input_column_vector = MakeShared<ColumnVector>(source_type);
        input_column_vector->Initialize();

        // Case 1
        String s1 = "100100100.01";
        Value v = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v);

        VarcharT* varchar_ptr = (VarcharT*)(input_column_vector->buffer_.get()->GetDataMut());

        DoubleT target{0};
        bool result = TryCastVarcharVector::Run(varchar_ptr[0], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, 100100100.01);

        // Case 2
        s1 = "-100.02";

        Value v1 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v1);

        result = TryCastVarcharVector::Run(varchar_ptr[1], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, -100.02);

        // Case 3
        s1 = "100000345345.345";

        Value v2 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v2);

        result = TryCastVarcharVector::Run(varchar_ptr[2], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, 100000345345.345);

        // Case 4
        s1 = "-1000000000345345.4565544";

        Value v3 = Value::MakeVarchar(s1);
        input_column_vector->AppendValue(v3);

        result = TryCastVarcharVector::Run(varchar_ptr[3], input_column_vector.get(), target);
        EXPECT_TRUE(result);
        EXPECT_FLOAT_EQ(target, -1000000000345345.4565544);
    }
}
