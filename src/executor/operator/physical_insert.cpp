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
import txn;
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

import column_def;

namespace infinity {

void PhysicalInsert::Init() {}

bool PhysicalInsert::Execute(QueryContext *query_context, OperatorState *operator_state) {
    SizeT row_count = value_list_.size();
    SizeT column_count = value_list_[0].size();
    SizeT table_collection_column_count = table_entry_->ColumnCount();
    if (column_count != table_collection_column_count) {
        String error_message = fmt::format("Insert values count{} isn't matched with table column count{}.", column_count, table_collection_column_count);
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

    SharedPtr<DataBlock> output_block = DataBlock::Make();
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

    auto *txn = query_context->GetTxn();
    txn->Append(table_entry_, output_block);

    UniquePtr<String> result_msg = MakeUnique<String>(fmt::format("INSERTED {} Rows", output_block->row_count()));
    if (operator_state == nullptr) {
        // Generate the result table
        Vector<SharedPtr<ColumnDef>> column_defs;
        SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default_db"), MakeShared<String>("Tables"), column_defs);
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
