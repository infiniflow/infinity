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

import stl;
import global_resource_usage;
import third_party;
import logger;
import data_table;
import table_def;
import value;
import parser;
import data_block;
import default_values;
import column_vector;
import infinity_context;

class TableTest : public BaseTest {};

TEST_F(TableTest, test1) {
    using namespace infinity;

    SizeT column_count = 2;
    SizeT block_count = 3;
    SizeT row_count = DEFAULT_VECTOR_SIZE;
    Vector<SharedPtr<ColumnDef>> columns;
    Vector<SharedPtr<DataType>> column_types;
    columns.reserve(column_count);
    column_types.reserve(column_count);

    SharedPtr<DataType> col_type = MakeShared<DataType>(LogicalType::kBoolean);
    column_types.emplace_back(col_type);
    String col_name = "col1";
    auto col_def = MakeShared<ColumnDef>(0, col_type, col_name, HashSet<ConstraintType>());

    columns.emplace_back(col_def);

    col_type = MakeShared<DataType>(LogicalType::kBigInt);
    column_types.emplace_back(col_type);
    col_name = "col2";
    col_def = MakeShared<ColumnDef>(1, col_type, col_name, HashSet<ConstraintType>());
    columns.emplace_back(col_def);

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("order_by_table"), columns);

    SharedPtr<DataTable> order_by_table = DataTable::Make(table_def, TableType::kOrderBy);

    for (SizeT block_id = 0; block_id < block_count; ++block_id) {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        data_block->Init(column_types);

        // Column 1 & 2
        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            Value v1 = Value::MakeBool(row_id % 2 == 0);
            data_block->column_vectors[0]->AppendValue(v1);
            Value v2 = Value::MakeBigInt(row_id);
            data_block->column_vectors[1]->AppendValue(v2);
        }
        data_block->Finalize();
        order_by_table->Append(data_block);
    }

    SharedPtr<Vector<RowID>> offset_column_vector = order_by_table->GetRowIDVector();
    EXPECT_EQ(offset_column_vector->size(), block_count * DEFAULT_VECTOR_SIZE);
    for (SizeT block_id = 0; block_id < block_count; ++block_id) {
        // Check Column1 data
        SharedPtr<ColumnVector> column1 = order_by_table->GetDataBlockById(block_id)->column_vectors[0];
        EXPECT_EQ(column1->data_type()->type(), LogicalType::kBoolean);
        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            EXPECT_EQ(((BooleanT *)column1->data())[row_id], row_id % 2 == 0);
        }

        // Check Column2 data
        SharedPtr<ColumnVector> column2 = order_by_table->GetDataBlockById(block_id)->column_vectors[1];
        EXPECT_EQ(column2->data_type()->type(), LogicalType::kBigInt);
        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            EXPECT_EQ(((BigIntT *)column2->data())[row_id], row_id);
        }

        // Check offset
        for (SizeT row_id = 0; row_id < row_count; ++row_id) {
            const RowID &row = (*offset_column_vector)[block_id * DEFAULT_VECTOR_SIZE + row_id];
            EXPECT_EQ(row.segment_id_, INVALID_SEGMENT_ID);
            EXPECT_EQ(row.segment_offset_, block_id * DEFAULT_VECTOR_SIZE + row_id);
        }
    }
}
