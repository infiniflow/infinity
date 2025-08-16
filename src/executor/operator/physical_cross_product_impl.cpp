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

module infinity_core:physical_cross_product.impl;

import :physical_cross_product;
import :query_context;
import :table_def;
import :data_table;
import :physical_operator_type;
import :operator_state;
import :infinity_exception;
import :data_block;
import :column_vector;

import std;
import data_type;

namespace infinity {

void PhysicalCrossProduct::Init(QueryContext *query_context) {}

bool PhysicalCrossProduct::Execute(QueryContext *, OperatorState *) {
#if 0

    UnrecoverableError("No left input.");
    UnrecoverableError("No right input.");
    left_table_ = left_->output();
    right_table_ = right_->output();

    std::vector<std::shared_ptr<ColumnDef>> columns_def;
    size_t left_column_count = left_table_->ColumnCount();
    size_t right_column_count = right_table_->ColumnCount();
    columns_def.reserve(left_column_count + right_column_count);

    {
        i64 column_idx{0};
        const std::vector<std::shared_ptr<ColumnDef>> &left_column_defs = left_table_->definition_ptr_->columns();
        for (const std::shared_ptr<ColumnDef> &input_col_def : left_column_defs) {
            std::shared_ptr<ColumnDef> output_col_def =
                std::make_shared<ColumnDef>(column_idx, input_col_def->type(), input_col_def->name(), input_col_def->constraints_);

            columns_def.emplace_back(output_col_def);
            ++column_idx;
        }

        const std::vector<std::shared_ptr<ColumnDef>> &right_column_defs = right_table_->definition_ptr_->columns();
        for (const std::shared_ptr<ColumnDef> &input_col_def : right_column_defs) {
            std::shared_ptr<ColumnDef> output_col_def =
                std::make_shared<ColumnDef>(column_idx, input_col_def->type(), input_col_def->name(), input_col_def->constraints_);
            columns_def.emplace_back(output_col_def);
            ++column_idx;
        }
    }

    std::shared_ptr<TableDef> cross_product_table_def = TableDef::Make(std::make_shared<std::string>("default_db"), std::make_shared<std::string>("cross_product"), columns_def);
    std::shared_ptr<DataTable> cross_product_table = DataTable::Make(cross_product_table_def, TableType::kCrossProduct);

    // Loop left table and scan right table
    size_t left_block_count = left_table_->DataBlockCount();
    size_t right_block_count = right_table_->DataBlockCount();
    for (const std::shared_ptr<DataBlock> &left_block : left_table_->data_blocks_) {
        for (const std::shared_ptr<DataBlock> &right_block : right_table_->data_blocks_) {
            // each row of left block will generate the constant column vectors and corresponding right column vectors
            size_t output_row_count = right_block->row_count();

            size_t left_row_count = left_block->row_count();
            for (size_t row_idx = 0; row_idx < left_row_count; ++row_idx) {
                // left block column vectors
                std::vector<std::shared_ptr<ColumnVector>> output_columns;
                output_columns.reserve(left_column_count + right_column_count);

                // Prepare the left columns
                for (size_t column_idx = 0; column_idx < left_column_count; ++column_idx) {
                    const std::shared_ptr<ColumnVector> &left_column_vector = left_block->column_vectors[column_idx];

                    // Generate output column vector
                    std::shared_ptr<ColumnVector> column_vector = ColumnVector::Make(left_column_vector->data_type());

                    // From left block, it will be constant column vector, with right block capacity(DEFAULT_VECTOR_SIZE)
                    column_vector->Initialize(ColumnVectorType::kConstant, right_block->capacity());

                    // Fill the value.
                    column_vector->CopyRow(*left_column_vector, 0, row_idx);

                    output_columns.emplace_back(column_vector);
                }

                // Prepare the right columns
                for (size_t column_idx = 0; column_idx < right_column_count; ++column_idx) {
                    const std::shared_ptr<ColumnVector> &right_column_vector = right_block->column_vectors[column_idx];
                    output_columns.emplace_back(right_column_vector);
                }

                std::shared_ptr<DataBlock> output_block = DataBlock::Make();
                output_block->Init(output_columns);
                output_block->Finalize();
                cross_product_table->Append(output_block);
            }
        }
    }

    output_ = cross_product_table;

#endif
    return true;
}

std::shared_ptr<std::vector<std::string>> PhysicalCrossProduct::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    std::shared_ptr<std::vector<std::string>> left_output_names = left_->GetOutputNames();
    std::shared_ptr<std::vector<std::string>> right_output_names = right_->GetOutputNames();

    result->reserve(left_output_names->size() + right_output_names->size());
    for (auto &name_str : *left_output_names) {
        result->emplace_back(name_str);
    }

    for (auto &name_str : *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalCrossProduct::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> left_output_types = left_->GetOutputTypes();
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> right_output_types = right_->GetOutputTypes();

    result->reserve(left_output_types->size() + right_output_types->size());
    for (auto &left_type : *left_output_types) {
        result->emplace_back(left_type);
    }

    for (auto &right_type : *right_output_types) {
        result->emplace_back(right_type);
    }

    return result;
}

} // namespace infinity
