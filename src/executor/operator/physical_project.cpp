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
        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx,
                                                             expr->Type(),
                                                             expr->Name(),
                                                             HashSet<ConstraintType>());
        projection_columns.emplace_back(col_def);

        // for output block
        output_types.emplace_back(expr->Type());

        ++ idx;
    }

    // output table definition
    SharedPtr<TableDef> projection_tabledef = TableDef::Make("projection", projection_columns);
    SharedPtr<Table> projection_table = Table::Make(projection_tabledef, TableType::kAggregate);

    if(left_ == nullptr) {
        Vector<SharedPtr<DataBlock>> empty_input_blocks;
        SharedPtr<DataBlock> output_data_block = DataBlock::Make();
        output_data_block->Init(output_types);
        ExpressionEvaluator evaluator;
        evaluator.Init(empty_input_blocks);
        for(SizeT expr_idx = 0; expr_idx < expression_count; ++ expr_idx) {
            Vector<SharedPtr<ColumnVector>> blocks_column;
            blocks_column.emplace_back(output_data_block->column_vectors[expr_idx]);
            evaluator.Execute(expressions_[expr_idx],
                              expr_states[expr_idx],
                              blocks_column);
            if(blocks_column[0].get() != output_data_block->column_vectors[expr_idx].get()) {
                // column vector in blocks column might be changed to the column vector from column reference.
                // This check and assignment is to make sure the right column vector are assign to output_data_block
                output_data_block->column_vectors[expr_idx] = blocks_column[0];
            }
        }
        output_data_block->Finalize();
        projection_table->Append(output_data_block);
    } else {
        // Get input from left child
        SharedPtr<Table> input_table = left_->output();
        ExecutorAssert(input_table != nullptr, "No input table for projection");
        // Get block count;
        SizeT input_block_count = input_table->DataBlockCount();

        // Loop blocks
        for(SizeT block_idx = 0; block_idx < input_block_count; ++ block_idx) {
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types);
            Vector<SharedPtr<DataBlock>> input_blocks{input_table->GetDataBlockById(block_idx)};

            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            evaluator.Init(input_blocks);
            for(SizeT expr_idx = 0; expr_idx < expression_count; ++ expr_idx) {
                Vector<SharedPtr<ColumnVector>> blocks_column;
                blocks_column.emplace_back(output_data_block->column_vectors[expr_idx]);
                evaluator.Execute(expressions_[expr_idx],
                                  expr_states[expr_idx],
                                  blocks_column);
                if(blocks_column[0].get() != output_data_block->column_vectors[expr_idx].get()) {
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

SharedPtr<Vector<String>>
PhysicalProject::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SizeT expression_count = expressions_.size();
    result->reserve(expression_count);
    for(SizeT i = 0; i < expression_count; ++ i) {
        result->emplace_back(expressions_[i]->Name());
    }

    return result;
}

}
