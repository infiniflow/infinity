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

import operator_state;
import expression_evaluator;
import expression_state;
import data_block;
import column_vector;

import infinity_exception;
import analyzer_pool;
import value;

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

bool PhysicalProject::Execute(QueryContext *, OperatorState *operator_state) {
    auto *project_operator_state = static_cast<ProjectionOperatorState *>(operator_state);
    if(project_operator_state->empty_source_) {
        project_operator_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock* output_data_block = project_operator_state->data_block_array_.back().get();
        output_data_block->Init(*GetOutputTypes());

        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);

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
            evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
        }
        output_data_block->Finalize();
        project_operator_state->SetComplete();
    } else {
        OperatorState* prev_op_state = operator_state->prev_op_state_;

        SizeT input_block_count = prev_op_state->data_block_array_.size();
        for(SizeT block_idx = 0; block_idx < input_block_count; ++ block_idx) {
            DataBlock* input_data_block = prev_op_state->data_block_array_[block_idx].get();

            project_operator_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
            DataBlock* output_data_block = project_operator_state->data_block_array_.back().get();
            output_data_block->Init(*GetOutputTypes());

            ExpressionEvaluator evaluator;
            evaluator.Init(input_data_block);

            SizeT expression_count = expressions_.size();

            // Prepare the expression states
            Vector<SharedPtr<ExpressionState>> expr_states;
            expr_states.reserve(expression_count);

            for (const auto &expr : expressions_) {
                // expression state
                expr_states.emplace_back(ExpressionState::CreateState(expr));
            }

            for (SizeT expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
                evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);

                auto it = highlight_columns_.find(expr_idx);
                if (it != highlight_columns_.end()) {
                    SizeT num_rows = output_data_block->column_vectors[expr_idx]->Size();
                    SharedPtr<ColumnVector> highlight_column = ColumnVector::Make(output_data_block->column_vectors[expr_idx]->data_type());
                    highlight_column->Initialize(ColumnVectorType::kFlat, num_rows);

                    SharedPtr<HighlightInfo> highlight_info = it->second;
                    Vector<String> &query_terms = highlight_info->query_terms_;
                    String &analyzer_name = highlight_info->analyzer_;
                    if (analyzer_name.find("standard") != std::string::npos) {
                        auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer(analyzer_name);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }
                        analyzer->SetCharOffset(true);
                        for (SizeT i = 0; i < num_rows; ++i) {
                            String raw_content = output_data_block->column_vectors[expr_idx]->GetValue(i).GetVarchar();
                            String output;
                            Highlighter::instance().GetHighlightWithStemmer(query_terms, raw_content, output, analyzer.get());
                            highlight_column->AppendValue(Value::MakeVarchar(output));
                        }
                    } else {
                        for (SizeT i = 0; i < num_rows; ++i) {
                            String raw_content = output_data_block->column_vectors[expr_idx]->GetValue(i).GetVarchar();
                            String output;
                            Highlighter::instance().GetHighlightWithoutStemmer(query_terms, raw_content, output);
                            highlight_column->AppendValue(Value::MakeVarchar(output));
                        }
                    }
                    output_data_block->column_vectors[expr_idx] = std::move(highlight_column);
                }
            }
            output_data_block->Finalize();
        }


        //    if (prev_op_state->Complete() && !prev_op_state->data_block_->Finalized()) {
        //        project_operator_state->data_block_->Finalize();
        //        project_operator_state->SetComplete();
        //        return ;
        //    }

        prev_op_state->data_block_array_.clear();
        if (prev_op_state->Complete()) {
            project_operator_state->SetComplete();
        }
    }

    return true;
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
