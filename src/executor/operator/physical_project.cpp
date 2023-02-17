//
// Created by JinHai on 2022/7/28.
//

#include "physical_project.h"
#include "executor/expression/expression_evaluator.h"

namespace infinity {

void
PhysicalProject::Init() {
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
//    outputs_[output_table_index_] = Table::Make(table_def, TableType::kIntermediate);
}

void
PhysicalProject::Execute(SharedPtr<QueryContext>& query_context) {
    if(left_ == nullptr) {
        NotImplementError("Only select list.")
    } else {
        // Get input from left child
        ExecutorAssert(left_->output() != nullptr, "No input table for projection");

        SizeT expression_count = expressions_.size();

        // Prepare the output table columns
        Vector<SharedPtr<ColumnDef>> projection_columns;
        projection_columns.reserve(expression_count);

        // Prepare the expression states
        Vector<SharedPtr<ExpressionState>> expr_states;
        expr_states.reserve(expression_count);

        // Prepare the output block
        Vector<DataType> output_types;
        output_types.reserve(expression_count);

        for(i64 idx = 0; auto& expr: expressions_) {
            // expression state
            expr_states.emplace_back(ExpressionState::CreateState(expr));

            // column definition
            SharedPtr<ColumnDef> col_def = ColumnDef::Make(expr->ToString(), idx, expr->Type(), Set<ConstrainType>());
            projection_columns.emplace_back(col_def);

            // for output block
            output_types.emplace_back(expr->Type());

            ++ idx;
        }

        // output table definition
        SharedPtr<TableDef> projection_tabledef = TableDef::Make("projection", projection_columns, false);
        SharedPtr<Table> projection_table = Table::Make(projection_tabledef, TableType::kAggregate);

        // Loop blocks
        SharedPtr<Table> input_table = left_->output();
        // Get block count;
        SizeT input_block_count = input_table->DataBlockCount();
        for(SizeT block_idx = 0; block_idx < input_block_count; ++ block_idx) {
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types);

            SharedPtr<DataBlock> input_data_block = input_table->GetDataBlockById(block_idx);
            SizeT input_block_row_count = -input_data_block->row_count();

            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            for(SizeT expr_idx = 0; expr_idx < expression_count; ++ expr_idx) {
                evaluator.Execute(expressions_[expr_idx],
                                  expr_states[expr_idx],
                                  input_data_block,
                                  input_block_row_count,
                                  output_data_block->column_vectors[expr_idx]);
            }

            output_data_block->Finalize();
            projection_table->Append(output_data_block);
        }

        output_ = projection_table;
    }
}

}
