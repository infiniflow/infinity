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

module;

#include <string>

module physical_insert;

import stl;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import expression_state;
import data_block;
import third_party;
import expression_evaluator;
import base_expression;
import default_values;
import status;
import infinity_exception;
import logger;
import meta_info;
import txn_state;

import wal_manager;
import infinity_context;

import column_def;
import new_txn;
import cast_function;
import bound_cast_func;

namespace infinity {

void PhysicalInsert::Init(QueryContext *query_context) {}

bool PhysicalInsert::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    SharedPtr<DataBlock> output_block;

    // Check if we have a child operator (INSERT SELECT) or direct values (INSERT VALUES)
    if (operator_state->prev_op_state_ != nullptr) {
        // INSERT SELECT case: get data from child operator
        OperatorState *prev_op_state = operator_state->prev_op_state_;
        SizeT data_block_count = prev_op_state->data_block_array_.size();

        if (data_block_count == 0) {
            // No data to insert
            UniquePtr<String> result_msg = MakeUnique<String>("INSERTED 0 Rows");
            if (operator_state == nullptr) {
                Vector<SharedPtr<ColumnDef>> column_defs;
                SharedPtr<TableDef> result_table_def_ptr =
                    TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
                output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
                output_->SetResultMsg(std::move(result_msg));
            } else {
                InsertOperatorState *insert_operator_state = static_cast<InsertOperatorState *>(operator_state);
                insert_operator_state->result_msg_ = std::move(result_msg);
            }
            operator_state->SetComplete();
            return true;
        }

        // Merge all data blocks from child operator
        output_block = DataBlock::Make();
        // Use target table column types for the output block
        Vector<SharedPtr<DataType>> target_types;
        for (SizeT i = 0; i < static_cast<SizeT>(table_info_->column_count_); ++i) {
            target_types.emplace_back(table_info_->column_defs_[i]->type());
        }
        output_block->Init(target_types);

        // Check if we need to apply type casting for INSERT SELECT
        DataBlock *first_block = prev_op_state->data_block_array_[0].get();
        bool needs_casting = false;
        for (SizeT i = 0; i < first_block->column_count() && i < target_types.size(); ++i) {
            if (*target_types[i] != *first_block->column_vectors[i]->data_type()) {
                needs_casting = true;
                break;
            }
        }

        if (needs_casting) {
            // Apply type casting for each block
            for (SizeT block_idx = 0; block_idx < data_block_count; ++block_idx) {
                DataBlock *input_data_block_ptr = prev_op_state->data_block_array_[block_idx].get();

                // Cast each column if needed
                for (SizeT col_idx = 0; col_idx < input_data_block_ptr->column_count() && col_idx < target_types.size(); ++col_idx) {
                    auto source_column = input_data_block_ptr->column_vectors[col_idx];
                    auto target_column = output_block->column_vectors[col_idx];
                    auto target_type = target_types[col_idx];

                    if (*source_column->data_type() == *target_type) {
                        // No casting needed, copy directly
                        target_column->AppendWith(*source_column, 0, source_column->Size());
                    } else {
                        // Apply standard casting (this will handle BigInt->Integer, etc.)
                        auto cast_func = CastFunction::GetBoundFunc(*source_column->data_type(), *target_type);
                        CastParameters cast_parameters;
                        cast_func.function(source_column, target_column, source_column->Size(), cast_parameters);
                    }
                }
            }
        } else {
            // No casting needed, append directly
            for (SizeT block_idx = 0; block_idx < data_block_count; ++block_idx) {
                DataBlock *input_data_block_ptr = prev_op_state->data_block_array_[block_idx].get();
                output_block->AppendWith(input_data_block_ptr);
            }
        }
        output_block->Finalize();

    } else {
        // INSERT VALUES case: evaluate expressions to create data block
        SizeT row_count = value_list_.size();
        if (row_count == 0) {
            String error_message = "No values to insert";
            UnrecoverableError(error_message);
        }

        SizeT column_count = value_list_[0].size();
        SizeT table_collection_column_count = table_info_->column_count_;
        if (column_count != table_collection_column_count) {
            String error_message =
                fmt::format("Insert values count{} isn't matched with table column count{}.", column_count, table_collection_column_count);
            UnrecoverableError(error_message);
        }

        // Prepare the output block
        Vector<SharedPtr<DataType>> output_types;
        output_types.reserve(column_count);
        auto field_list = value_list_[0];
        SizeT field_count = field_list.size();
        for (SizeT i = 0; i < field_count; ++i) {
            auto data_type = field_list[i]->Type();
            output_types.emplace_back(MakeShared<DataType>(data_type));
        }

        output_block = DataBlock::Make();
        output_block->Init(output_types);
        SharedPtr<DataBlock> output_block_tmp = DataBlock::Make();
        output_block_tmp->Init(output_types);

        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);
        // Each cell's expression of a column may differ. So we have to evaluate each cell instead of column here.
        for (SizeT row_idx = 0; row_idx < row_count; ++row_idx) {
            for (SizeT expr_idx = 0; expr_idx < column_count; ++expr_idx) {
                const SharedPtr<BaseExpression> &expr = value_list_[row_idx][expr_idx];
                SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(expr);
                evaluator.Execute(expr, expr_state, output_block_tmp->column_vectors[expr_idx]);
            }
            output_block->AppendWith(output_block_tmp);
        }
        output_block->Finalize();
    }

    NewTxn *new_txn = query_context->GetNewTxn();
    new_txn->SetTxnType(TransactionType::kAppend);
    Status status = new_txn->Append(*table_info_, output_block);
    if (!status.ok()) {
        operator_state->status_ = status;
    }

    UniquePtr<String> result_msg = MakeUnique<String>(fmt::format("INSERTED {} Rows", output_block->row_count()));
    if (operator_state == nullptr) {
        // Generate the result table
        Vector<SharedPtr<ColumnDef>> column_defs;
        SharedPtr<TableDef> result_table_def_ptr =
            TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
        output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
        output_->SetResultMsg(std::move(result_msg));
    } else {
        InsertOperatorState *insert_operator_state = static_cast<InsertOperatorState *>(operator_state);
        insert_operator_state->result_msg_ = std::move(result_msg);
    }
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
