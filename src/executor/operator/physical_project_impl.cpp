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

module infinity_core:physical_project.impl;

import :physical_project;
import :query_context;
import :table_def;
import :data_table;
import :operator_state;
import :expression_evaluator;
import :expression_state;
import :data_block;
import :column_vector;
import :infinity_exception;
import :analyzer_pool;
import :rag_analyzer;
import :value;
import :physical_project;
import :highlighter;

import std;

import data_type;

namespace infinity {

void PhysicalProject::Init(QueryContext *query_context) {
    //    executor.Init(expressions_);
    //
    //    std::vector<std::shared_ptr<ColumnDef>> columns;
    //    for(i64 idx = 0; auto& expr: expressions_) {
    //        std::shared_ptr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
    //        columns.emplace_back(col_def);
    //        ++ idx;
    //    }
    //    std::shared_ptr<TableDef> table_def = TableDef::Make("project", columns, false);
    //
    //    outputs_[output_table_index_] = DataTable::Make(table_def, TableType::kIntermediate);
}

bool PhysicalProject::Execute(QueryContext *, OperatorState *operator_state) {
    auto *project_operator_state = static_cast<ProjectionOperatorState *>(operator_state);
    if (project_operator_state->empty_source_) {
        project_operator_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock *output_data_block = project_operator_state->data_block_array_.back().get();
        output_data_block->Init(*GetOutputTypes());

        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);

        size_t expression_count = expressions_.size();

        // Prepare the expression states
        std::vector<std::shared_ptr<ExpressionState>> expr_states;
        expr_states.reserve(expression_count);

        for (const auto &expr : expressions_) {
            // expression state
            expr_states.emplace_back(ExpressionState::CreateState(expr));
        }

        for (size_t expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
            //        std::vector<std::shared_ptr<ColumnVector>> blocks_column;
            //        blocks_column.emplace_back(output_data_block->column_vectors[expr_idx]);
            evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
        }
        output_data_block->Finalize();
        project_operator_state->SetComplete();
    } else {
        OperatorState *prev_op_state = operator_state->prev_op_state_;

        size_t input_block_count = prev_op_state->data_block_array_.size();
        for (size_t block_idx = 0; block_idx < input_block_count; ++block_idx) {
            DataBlock *input_data_block = prev_op_state->data_block_array_[block_idx].get();

            project_operator_state->data_block_array_.emplace_back(DataBlock::MakeUniquePtr());
            DataBlock *output_data_block = project_operator_state->data_block_array_.back().get();
            output_data_block->Init(*GetOutputTypes());

            ExpressionEvaluator evaluator;
            evaluator.Init(input_data_block);

            size_t expression_count = expressions_.size();

            // Prepare the expression states
            std::vector<std::shared_ptr<ExpressionState>> expr_states;
            expr_states.reserve(expression_count);

            for (const auto &expr : expressions_) {
                // expression state
                expr_states.emplace_back(ExpressionState::CreateState(expr));
            }

            for (size_t expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
                evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
            }

            if (!highlight_columns_.empty()) {
                for (size_t expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
                    auto it = highlight_columns_.find(expr_idx);
                    if (it != highlight_columns_.end()) {
                        size_t num_rows = output_data_block->column_vectors[expr_idx]->Size();
                        std::shared_ptr<ColumnVector> highlight_column = ColumnVector::Make(output_data_block->column_vectors[expr_idx]->data_type());
                        highlight_column->Initialize(ColumnVectorType::kFlat, num_rows);
                        std::shared_ptr<HighlightInfo> highlight_info = it->second;

                        auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer(highlight_info->analyzer_);
                        if (!status.ok()) {
                            RecoverableError(status);
                        }

                        RAGAnalyzer *rag_analyzer = dynamic_cast<RAGAnalyzer *>(analyzer.get());
                        if (!rag_analyzer) {
                            UnrecoverableError(fmt::format("RAGAnalyzer should be used for highlight. Analyzer is : {}", highlight_info->analyzer_));
                        }
                        rag_analyzer->SetEnablePosition(true);

                        for (size_t i = 0; i < num_rows; ++i) {
                            std::string raw_content = output_data_block->column_vectors[expr_idx]->GetValueByIndex(i).GetVarchar();
                            std::string output;
                            Highlighter::instance().GetHighlight(highlight_info->matching_text_, raw_content, output, analyzer.get());
                            highlight_column->AppendValue(Value::MakeVarchar(output));
                        }
                        output_data_block->column_vectors[expr_idx] = std::move(highlight_column);
                    }
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
            if (prev_op_state->total_hits_count_flag_) {
                project_operator_state->total_hits_count_flag_ = true;
                project_operator_state->total_hits_count_ = prev_op_state->total_hits_count_;
            }
            project_operator_state->SetComplete();
        }
    }

    return true;
}

std::shared_ptr<std::vector<std::string>> PhysicalProject::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    size_t expression_count = expressions_.size();
    result->reserve(expression_count);
    for (size_t i = 0; i < expression_count; ++i) {
        result->emplace_back(expressions_[i]->Name());
    }

    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalProject::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    size_t expression_count = expressions_.size();
    result->reserve(expression_count);
    for (size_t i = 0; i < expression_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(expressions_[i]->Type()));
    }

    return result;
}

} // namespace infinity
