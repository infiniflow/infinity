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

module infinity_core:ut.decimal_cast;

import :ut.base_test;
import :infinity_exception;
import :third_party;
import :logger;
import :stl;
import :infinity_context;
import :function_set;
import :aggregate_function_set;
import :aggregate_function;
import :function;
import :column_expression;
import :value;
import :default_values;
import :data_block;
import :cast_table;
import :column_vector;
import :decimal_cast;
import :bound_cast_func;
#endif

import global_resource_usage;
import internal_types;
import logical_type;
import data_type;

using namespace infinity;

class DecimalCastTest : public BaseTest {};

TEST_F(DecimalCastTest, decimal_cast0) {
    using namespace infinity;

    // DecimalT to TinyIntT, throw exception
    {
        DecimalT source;
        TinyIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to SmallIntT, throw exception
    {
        DecimalT source;
        SmallIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to IntegerT, throw exception
    {
        DecimalT source;
        IntegerT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to BigIntT, throw exception
    {
        DecimalT source;
        BigIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to HugeIntT, throw exception
    {
        DecimalT source;
        HugeIntT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to FloatT, throw exception
    {
        DecimalT source;
        FloatT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to DoubleT, throw exception
    {
        DecimalT source;
        DoubleT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to DecimalT, throw exception
    {
        DecimalT source;
        DecimalT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToFixlen::Run(source, target), UnrecoverableException);
    }

    // DecimalT to VarcharT, throw exception
    {
        DecimalT source;
        VarcharT target;
        EXPECT_THROW_WITHOUT_STACKTRACE(DecimalTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
}

TEST_F(DecimalCastTest, decimal_cast1) {
    using namespace infinity;

    // cast decimal column vector to tiny int column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kTinyInt);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to small int column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kSmallInt);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to integer column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kInteger);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to bigint column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kBigInt);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to hugeint column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kHugeInt);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to float column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kFloat);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to double column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kDouble);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to decimal column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
    // cast decimal column vector to varchar column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kVarchar);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }

    // cast decimal column vector to other type column vector
    {
        DataType source_type(LogicalType::kDecimal);
        DataType target_type(LogicalType::kTimestamp);
        EXPECT_THROW_WITHOUT_STACKTRACE(BindDecimalCast<DecimalT>(source_type, target_type), UnrecoverableException);
    }
}
