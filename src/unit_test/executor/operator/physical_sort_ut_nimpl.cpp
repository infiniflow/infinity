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
// import :ut.sql_runner;

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.physical_sort;

import :ut.base_test;
import :ut.sql_runner;
import :infinity_exception;
import third_party;
import :logger;
import :infinity_context;
import :default_values;
import :data_table;
import :table_def;
import :value;
import :data_block;
import :physical_sort;

import global_resource_usage;

#if 0
using namespace infinity;
class PhysicalSortTest : public BaseTest {};

TEST_F(PhysicalSortTest, test1) {
    using namespace infinity;

    size_t column_count = 2;
    size_t block_count = 3;
    size_t row_count = DEFAULT_VECTOR_SIZE;
    Vector<std::shared_ptr<ColumnDef>> columns;
    Vector<std::shared_ptr<DataType>> column_types;
    columns.reserve(column_count);
    column_types.reserve(column_count);

    std::shared_ptr<DataType> col_type = std::make_shared<DataType>(LogicalType::kBoolean);
    column_types.emplace_back(col_type);
    String col_name = "col1";
    std::shared_ptr<ColumnDef> col_def = std::make_shared<ColumnDef>(0,
                                                         col_type,
                                                         col_name,
                                                         std::set<ConstraintType>());
    columns.emplace_back(col_def);

    col_type = std::make_shared<DataType>(LogicalType::kBigInt);
    column_types.emplace_back(col_type);
    col_name = "col2";
    col_def = std::make_shared<ColumnDef>(1,
                                    col_type,
                                    col_name,
                                    std::set<ConstraintType>());
    columns.emplace_back(col_def);

    std::shared_ptr<TableDef> table_def = TableDef::Make(std::make_shared<String>("default_db"),
                                                   std::make_shared<String>("input_table"),
                                                   columns);

    std::shared_ptr<DataTable> input_table = DataTable::Make(table_def, TableType::kIntermediate);

    for(size_t block_id = 0; block_id < block_count; ++block_id) {
        std::shared_ptr<DataBlock> data_block = DataBlock::Make();
        data_block->Init(column_types);

        // Column 1 & 2
        for(size_t row_id = 0; row_id < row_count; ++row_id) {
            Value v1 = Value::MakeBool(row_id % 2 == 0);
            data_block->column_vectors[0]->AppendValue(v1);
            Value v2 = Value::MakeBigInt(row_id + block_id * 10000);
            data_block->column_vectors[1]->AppendValue(v2);
        }
        data_block->Finalize();
        input_table->Append(data_block);
    }

    std::shared_ptr<Vector<RowID>> rowid_vector = std::make_shared<Vector<RowID>>();
    rowid_vector->reserve(block_count * DEFAULT_VECTOR_SIZE);
    for(i64 block_id = block_count - 1; block_id >= 0; --block_id) {
        for(i64 row_id = DEFAULT_VECTOR_SIZE - 1; row_id >= 0; --row_id) {
            rowid_vector->emplace_back(RowID(INVALID_SEGMENT_ID, block_id, row_id));
        }
    }

    std::shared_ptr<DataTable> output_table = PhysicalSort::GenerateOutput(input_table, rowid_vector);
    std::cout << output_table->ToString() << std::endl;
}
#endif
