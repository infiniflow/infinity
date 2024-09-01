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

#if 0
#include "gtest/gtest.h"
import base_test;

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
import physical_sort;
//import sql_runner;

using namespace infinity;
class PhysicalSortTest : public BaseTest {};

TEST_F(PhysicalSortTest, test1) {
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
    SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(0,
                                                         col_type,
                                                         col_name,
                                                         std::set<ConstraintType>());
    columns.emplace_back(col_def);

    col_type = MakeShared<DataType>(LogicalType::kBigInt);
    column_types.emplace_back(col_type);
    col_name = "col2";
    col_def = MakeShared<ColumnDef>(1,
                                    col_type,
                                    col_name,
                                    std::set<ConstraintType>());
    columns.emplace_back(col_def);

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"),
                                                   MakeShared<String>("input_table"),
                                                   columns);

    SharedPtr<DataTable> input_table = DataTable::Make(table_def, TableType::kIntermediate);

    for(SizeT block_id = 0; block_id < block_count; ++block_id) {
        SharedPtr<DataBlock> data_block = DataBlock::Make();
        data_block->Init(column_types);

        // Column 1 & 2
        for(SizeT row_id = 0; row_id < row_count; ++row_id) {
            Value v1 = Value::MakeBool(row_id % 2 == 0);
            data_block->column_vectors[0]->AppendValue(v1);
            Value v2 = Value::MakeBigInt(row_id + block_id * 10000);
            data_block->column_vectors[1]->AppendValue(v2);
        }
        data_block->Finalize();
        input_table->Append(data_block);
    }

    SharedPtr<Vector<RowID>> rowid_vector = MakeShared<Vector<RowID>>();
    rowid_vector->reserve(block_count * DEFAULT_VECTOR_SIZE);
    for(i64 block_id = block_count - 1; block_id >= 0; --block_id) {
        for(i64 row_id = DEFAULT_VECTOR_SIZE - 1; row_id >= 0; --row_id) {
            rowid_vector->emplace_back(RowID(INVALID_SEGMENT_ID, block_id, row_id));
        }
    }

    SharedPtr<DataTable> output_table = PhysicalSort::GenerateOutput(input_table, rowid_vector);
    std::cout << output_table->ToString() << std::endl;
}
#endif
