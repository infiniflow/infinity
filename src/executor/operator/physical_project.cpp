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
import stl;
import query_context;
import table_def;
import data_table;
import parser;
import operator_state;
import expression_evaluator;
import expression_state;
import data_block;
import column_vector;

import infinity_exception;

module physical_project;

namespace infinity {

void PhysicalProject::Init() {
    //    executor.Init(expressions_);
    //
    //    Vector<SharedPtr<ColumnDef>> columns;
    //    for(i64 idx = 0; auto& expr: expressions_) {
    //        SharedPtr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
    //        columns.emplace_back(col_def);
    //        ++ idx;
    //    }
    //    SharedPtr<TableDef> table_def = TableDef::Make("project", columns, false);
    //
    //    outputs_[output_table_index_] = DataTable::Make(table_def, TableType::kIntermediate);
}

void PhysicalProject::Execute(QueryContext *query_context, OperatorState *operator_state) {
    OperatorState* prev_op_state = operator_state->prev_op_state_;
    auto *project_operator_state = static_cast<ProjectionOperatorState *>(operator_state);
    if (prev_op_state->Complete() && !prev_op_state->data_block_->Finalized()) {
        project_operator_state->data_block_->Finalize();
        project_operator_state->SetComplete();
        return ;
    }

    // FIXME: need to handle statement like: SELECT 1;

    // Should not reset here. Reset has be done in sink operator.
    // project_output_state->data_block_->Reset();

    // Loop aggregate expression
    ExpressionEvaluator evaluator;
    evaluator.Init(prev_op_state->data_block_.get());

    SizeT expression_count = expressions_.size();

    // Prepare the expression states
    Vector<SharedPtr<ExpressionState>> expr_states;
    expr_states.reserve(expression_count);

    for (const auto &expr : expressions_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }

    for (SizeT expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
        //        Vector<SharedPtr<ColumnVector>> blocks_column;
        //        blocks_column.emplace_back(output_data_block->column_vectors[expr_idx]);
        evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], project_operator_state->data_block_->column_vectors[expr_idx]);
    }

    project_operator_state->data_block_->Finalize();
    prev_op_state->data_block_->Reset();
    if (prev_op_state->Complete()) {
        project_operator_state->SetComplete();
    }
}

SharedPtr<Vector<String>> PhysicalProject::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SizeT expression_count = expressions_.size();
    result->reserve(expression_count);
    for (SizeT i = 0; i < expression_count; ++i) {
        result->emplace_back(expressions_[i]->Name());
    }

    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalProject::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    SizeT expression_count = expressions_.size();
    result->reserve(expression_count);
    for (SizeT i = 0; i < expression_count; ++i) {
        result->emplace_back(MakeShared<DataType>(expressions_[i]->Type()));
    }

    return result;
}

} // namespace infinity
