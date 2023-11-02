//
// Created by JinHai on 2022/7/28.
//

module;

import std;
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
import infinity_assert;
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

void PhysicalProject::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto *project_input_state = static_cast<ProjectionInputState *>(input_state);
    auto *project_output_state = static_cast<ProjectionOutputState *>(output_state);

    // FIXME: need to handle statement like: SELECT 1;

    // Should not reset here. Reset has be done in sink operator.
    // project_output_state->data_block_->Reset();

    // Loop aggregate expression
    ExpressionEvaluator evaluator;
    evaluator.Init(project_input_state->input_data_block_);

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
        evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], project_output_state->data_block_->column_vectors[expr_idx]);
    }

    project_output_state->data_block_->Finalize();
    if (project_input_state->Complete()) {
        project_output_state->SetComplete();
    }
}

void PhysicalProject::Execute(QueryContext *query_context) {
    SizeT expression_count = expressions_.size();

    // Prepare the output table columns
    Vector<SharedPtr<ColumnDef>> projection_columns;
    projection_columns.reserve(expression_count);

    // Prepare the expression states
    Vector<SharedPtr<ExpressionState>> expr_states;
    expr_states.reserve(expression_count);

    // Prepare the output block
    Vector<SharedPtr<DataType>> output_types;
    output_types.reserve(expression_count);

    for (i64 idx = 0; auto &expr : expressions_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(expr));
        SharedPtr<DataType> data_type = MakeShared<DataType>(expr->Type());

        // column definition
        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, data_type, expr->Name(), HashSet<ConstraintType>());
        projection_columns.emplace_back(col_def);

        // for output block
        output_types.emplace_back(data_type);

        ++idx;
    }

    // output table definition
    SharedPtr<TableDef> projection_tabledef = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("projection"), projection_columns);
    SharedPtr<DataTable> projection_table = DataTable::Make(projection_tabledef, TableType::kAggregate);

    if (left_.get() == nullptr) {
        Vector<SharedPtr<DataBlock>> empty_input_blocks;
        SharedPtr<DataBlock> output_data_block = DataBlock::Make();
        output_data_block->Init(output_types);
        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);
        for (SizeT expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
            Vector<SharedPtr<ColumnVector>> blocks_column;
            blocks_column.emplace_back(output_data_block->column_vectors[expr_idx]);
            evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
        }
        output_data_block->Finalize();
        projection_table->Append(output_data_block);
    } else {
        // Get input from left child
        SharedPtr<DataTable> input_table = left_->output();
        Assert<ExecutorException>(input_table.get() != nullptr, "No input table for projection", __FILE_NAME__, __LINE__);
        // Get block count;
        SizeT input_block_count = input_table->DataBlockCount();

        // Loop blocks
        for (SizeT block_idx = 0; block_idx < input_block_count; ++block_idx) {
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types);

            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            evaluator.Init(input_table->GetDataBlockById(block_idx).get());
            for (SizeT expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
                Vector<SharedPtr<ColumnVector>> blocks_column;
                evaluator.Execute(expressions_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
                if (blocks_column[0].get() != output_data_block->column_vectors[expr_idx].get()) {
                    // column vector in blocks column might be changed to the column vector from column reference.
                    // This check and assignment is to make sure the right column vector are assign to output_data_block
                    output_data_block->column_vectors[expr_idx] = blocks_column[0];
                }
            }

            output_data_block->Finalize();
            projection_table->Append(output_data_block);
        }
    }
    output_ = projection_table;
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
