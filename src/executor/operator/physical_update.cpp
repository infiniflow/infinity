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

module physical_update;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import query_context;
// import data_table;
import operator_state;
import data_block;
import column_vector;
import expression_evaluator;
import expression_state;
import expression_type;
import base_expression;
import logical_type;
import internal_types;
import value;

import wal_manager;
import infinity_context;
import status;
import txn;
import new_txn;

namespace infinity {

void PhysicalUpdate::Init(QueryContext* query_context) {}

bool PhysicalUpdate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    Txn *txn = query_context->GetTxn();
    NewTxn *new_txn = query_context->GetNewTxn();

    OperatorState *prev_op_state = operator_state->prev_op_state_;
    SizeT input_data_block_count = prev_op_state->data_block_array_.size();

    bool use_new_catalog = query_context->global_config()->UseNewCatalog();

    for (SizeT block_idx = 0; block_idx < input_data_block_count; ++block_idx) {
        DataBlock *input_data_block_ptr = prev_op_state->data_block_array_[block_idx].get();
        Vector<RowID> row_ids;
        for (SizeT i = 0; i < input_data_block_ptr->column_count(); i++) {
            if (auto &column_vector = input_data_block_ptr->column_vectors[i]; column_vector->data_type()->type() == LogicalType::kRowID) {
                row_ids.resize(column_vector->Size());
                std::memcpy(row_ids.data(), column_vector->data(), column_vector->Size() * sizeof(RowID));
                break;
            }
        }
        if (!row_ids.empty()) {
            Vector<SharedPtr<ColumnVector>> output_column_vectors(final_result_columns_.size());
            ExpressionEvaluator evaluator;
            evaluator.Init(input_data_block_ptr);
            for (SizeT i = 0; i < final_result_columns_.size(); ++i) {
                auto &output_column_vector = output_column_vectors[i];
                const auto &expr = final_result_columns_[i];
                SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(expr);
                if (expr->type() == ExpressionType::kReference) {
                    evaluator.Execute(expr, expr_state, output_column_vector);
                } else {
                    output_column_vector = ColumnVector::Make(MakeShared<DataType>(expr->Type()));
                    output_column_vector->Initialize();
                    evaluator.Execute(expr, expr_state, output_column_vector);
                    if (output_column_vector->Size() != input_data_block_ptr->row_count()) {
                        // output_column_vector only have one row;
                        if (output_column_vector->Size() != 1) {
                            UnrecoverableError("Expect the column vector has only one row");
                            return false;
                        }
                        Value value = output_column_vector->GetValue(0);
                        SizeT row_count = input_data_block_ptr->row_count();
                        output_column_vector = ColumnVector::Make(MakeShared<DataType>(expr->Type()));
                        output_column_vector->Initialize();
                        for (SizeT row_idx = 0; row_idx < row_count; ++row_idx) {
                            output_column_vector->AppendValue(value);
                        }
                        output_column_vector->Finalize(row_count);
                    }
                }
            }
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_column_vectors);

            if (use_new_catalog) {
                Status status = new_txn->Append(*table_info_->db_name_, *table_info_->table_name_, output_data_block);
                if (!status.ok()) {
                    operator_state->status_ = status;
                    RecoverableError(status);
                    return false;
                }
                status = new_txn->Delete(*table_info_->db_name_, *table_info_->table_name_, row_ids);
                if (!status.ok()) {
                    operator_state->status_ = status;
                    RecoverableError(status);
                    return false;
                }
            } else {
                txn->Append(*table_info_->db_name_, *table_info_->table_name_, output_data_block);
                txn->Delete(*table_info_->db_name_, *table_info_->table_name_, row_ids);
            }

            UpdateOperatorState *update_operator_state = static_cast<UpdateOperatorState *>(operator_state);
            ++update_operator_state->count_;
            update_operator_state->sum_ += row_ids.size();
        }
    }

    prev_op_state->data_block_array_.clear();
    if (prev_op_state->Complete()) {
        operator_state->SetComplete();
    }
    return true;
}

} // namespace infinity
