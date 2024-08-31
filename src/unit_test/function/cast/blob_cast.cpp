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

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;


import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import cast_table;
import column_vector;
import blob_cast;
import bound_cast_func;
import internal_types;
import logical_type;
#if 0
using namespace infinity;
class BlobCastTest : public BaseTest {};

TEST_F(BlobCastTest, blob_cast0) {
    using namespace infinity;

    // Try to cast blob type to wrong type.
    {
        i64 blob_len = 128;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT source(blob_ptr, blob_len);

        TinyIntT target;
        EXPECT_THROW(BlobTryCastToVarlen::Run(source, target, nullptr), UnrecoverableException);
    }
    {
        i64 blob_len = 128;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT source(blob_ptr, blob_len);
        VarcharT target;

        SharedPtr<DataType> data_type = MakeShared<DataType>(LogicalType::kVarchar);
        SharedPtr<ColumnVector> col_varchar_ptr = MakeShared<ColumnVector>(data_type);
        col_varchar_ptr->Initialize();

        EXPECT_TRUE(BlobTryCastToVarlen::Run(source, target, col_varchar_ptr.get()));
        target.Reset(false);
    }
}

TEST_F(BlobCastTest, blob_cast1) {
    using namespace infinity;

    // Call BindBlobCast with wrong type of parameters
    {
        DataType target_type(LogicalType::kDecimal);
        EXPECT_THROW(BindBlobCast(target_type), UnrecoverableException);
    }

    SharedPtr<DataType> source_type = MakeShared<DataType>(LogicalType::kBlob);
    SharedPtr<ColumnVector> col_source = MakeShared<ColumnVector>(source_type);
    col_source->Initialize();
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value v = Value::MakeBlob(b1);
        col_source->AppendValue(v);
    }
    for (i64 i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
        i64 blob_len = i + 1;
        auto blob_ptr = new char[blob_len]{0};
        // GlobalResourceUsage::IncrRawMemCount();

        for (i64 j = 0; j < blob_len; ++j) {
            blob_ptr[j] = 'a' + static_cast<char_t>(j);
        }
        blob_ptr[blob_len - 1] = 0;
        BlobT b1(blob_ptr, blob_len);

        Value vx = col_source->GetValue(i);
        EXPECT_EQ(vx.type().type(), LogicalType::kBlob);
        EXPECT_EQ(vx.value_.blob, b1);
    }
    // cast blob column vector to varchar column vector
    {
        SharedPtr<DataType> target_type = MakeShared<DataType>(LogicalType::kVarchar);
        auto source2target_ptr = BindBlobCast(*target_type);
        EXPECT_NE(source2target_ptr.function, nullptr);

        SharedPtr<ColumnVector> col_target = MakeShared<ColumnVector>(target_type);
        col_target->Initialize();

        CastParameters cast_parameters;
        EXPECT_TRUE(source2target_ptr.function(col_source, col_target, DEFAULT_VECTOR_SIZE, cast_parameters));

        for (i64 i = 99; i < 100; ++i) {
            i64 blob_len = i + 1;
            auto check_value = String();
            check_value.resize(blob_len);

            for (i64 j = 0; j < blob_len; ++j) {
                check_value[j] = 'a' + static_cast<char_t>(j);
            }
            check_value[blob_len - 1] = 0;
            Value vx = col_target->GetValue(i);
            const String &s2 = vs.GetString();
            EXPECT_EQ(s2, check_value);
        }
    }
}
#endif