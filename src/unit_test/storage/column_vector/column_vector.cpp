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
import parser;
import default_values;
import third_party;
import stl;
import global_resource_usage;
import infinity_context;

class ColumnVectorTest : public BaseTest {};

TEST_F(ColumnVectorTest, ReadWrite) {
    using namespace infinity;

    Vector<SharedPtr<ColumnVector>> columns;
    int vector_size = DEFAULT_VECTOR_SIZE / 3;
    {
        SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kBoolean));
        column->Initialize();
        for (i64 i = 0; i < vector_size; ++i) {
            Value v = Value::MakeBool(bool(i % 2));
            column->AppendValue(v);
        }
        columns.push_back(column);
    }
    {
        SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kFloat));
        column->Initialize();
        for (i64 i = 0; i < vector_size; ++i) {
            Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
            column->AppendValue(v);
        }
        columns.push_back(column);
    }
    {
        SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kFloat));
        column->Initialize(ColumnVectorType::kConstant);
        for (i64 i = 0; i < 1; ++i) {
            Value v = Value::MakeFloat(static_cast<FloatT>(i) + 0.5f);
            column->AppendValue(v);
        }
        columns.push_back(column);
    }

    /*
    // TODO: Following case cause exception due to the value created by `Value::MakeBitmap` has null info_ptr.
         {
            SharedPtr<ColumnVector> column =
                ColumnVector::Make(MakeShared<DataType>(LogicalType::kBitmap,
       BitmapInfo::Make(1024))); column->Initialize(); for (i64 i = 0; i <
       vector_size; ++i) { BitmapT bitmap; bitmap.Initialize(i + 10); for (i64 j
       = 0; j <= i; ++j) { if (j % 2 == 0) { bitmap.SetBit(j, true); } else {
                        bitmap.SetBit(j, false);
                    }
                }
                Value v = Value::MakeBitmap(bitmap);
                column->AppendValue(v);
            }
            columns.push_back(column);
        }
     */
    for (int i = 0; i < columns.size(); i++) {
        int32_t exp_size = columns[i]->GetSizeInBytes();
        int32_t act_size;
        Vector<char> buf(exp_size);
        char *ptr = buf.data();

        columns[i]->WriteAdv(ptr);
        act_size = ptr - buf.data();
        EXPECT_EQ(act_size, exp_size);

        ptr = buf.data();
        SharedPtr<ColumnVector> column2 = ColumnVector::ReadAdv(ptr, exp_size);
        act_size = ptr - buf.data();
        EXPECT_EQ(act_size, exp_size);
        EXPECT_NE(column2, nullptr);
        EXPECT_EQ(*columns[i] == *column2, true);
    }
}
