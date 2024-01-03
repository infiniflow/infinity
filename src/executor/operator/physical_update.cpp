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

import stl;
#include <string>
import parser;
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
import base_expression;

module physical_update;

namespace infinity {

void PhysicalUpdate::Init() {}

bool PhysicalUpdate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    OperatorState* prev_op_state = operator_state->prev_op_state_;
    SizeT input_data_block_count = prev_op_state->data_block_array_.size();
    for(SizeT block_idx = 0; block_idx < input_data_block_count; ++ block_idx) {
        DataBlock *input_data_block_ptr = prev_op_state->data_block_array_[block_idx].get();

        auto txn = query_context->GetTxn();
        const String& db_name = *table_entry_ptr_->GetDBName();
        auto table_name = table_entry_ptr_->GetTableName();
        Vector<RowID> row_ids;
        Vector<SharedPtr<ColumnVector>> column_vectors;
        for (SizeT i = 0; i < input_data_block_ptr->column_count(); i++) {
            SharedPtr<ColumnVector> column_vector = input_data_block_ptr->column_vectors[i];
            if (column_vector->data_type()->type() == LogicalType::kRowID) {
                row_ids.resize(column_vector->Size());
                Memcpy(row_ids.data(), column_vector->data(), column_vector->Size() * sizeof(RowID));
                break;
            } else {
                column_vectors.push_back(column_vector);
            }
        }
        if (!row_ids.empty()) {
            ExpressionEvaluator evaluator;
            evaluator.Init(input_data_block_ptr);
            for (SizeT expr_idx = 0; expr_idx < update_columns_.size(); ++expr_idx) {
                SizeT column_idx = update_columns_[expr_idx].first;
                const SharedPtr<BaseExpression> &expr = update_columns_[expr_idx].second;
                SharedPtr<ColumnVector> output_column = ColumnVector::Make(column_vectors[column_idx]->data_type());
                output_column->Initialize(ColumnVectorType::kFlat);
                SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(expr);
                evaluator.Execute(expr, expr_state, output_column);
                column_vectors[column_idx] = output_column;
            }

            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(column_vectors);
            txn->Append(db_name, *table_name, output_data_block);
            txn->Delete(db_name, *table_name, row_ids);

            UpdateOperatorState* update_operator_state = static_cast<UpdateOperatorState*>(operator_state);
            ++ update_operator_state->count_;
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
