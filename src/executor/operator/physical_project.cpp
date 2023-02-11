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
        ExecutorAssert(!left()->outputs().empty(), "No input table for projection");

        // Validate input and get block count;
        SizeT input_block_count{};
        for(bool first = true; const auto& input_table: left()->outputs()) {
            if(first) {
                input_block_count = input_table.second->DataBlockCount();
                first = false;
            } else {
                ExecutorAssert(input_block_count == input_table.second->DataBlockCount(), "Table block count mismatches");
            }
        }

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
        HashMap<u64, SharedPtr<DataBlock>> block_map;
        for(SizeT block_idx = 0; block_idx < input_block_count; ++ block_idx) {
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types);

            SizeT block_row_count{};
            for(const auto& input_table: left()->outputs()) {
                u64 table_index = input_table.first;
                const SharedPtr<Table>& table_ptr = input_table.second;
                const SharedPtr<DataBlock>& block_ptr = table_ptr->GetDataBlockById(block_idx);
                block_map[table_index] = block_ptr;
                block_row_count = block_ptr->row_count();
            }

            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            for(SizeT expr_idx = 0; expr_idx < expression_count; ++ expr_idx) {
                evaluator.Execute(expressions_[expr_idx],
                                  expr_states[expr_idx],
                                  block_map,
                                  block_row_count,
                                  output_data_block->column_vectors[expr_idx]);
            }

            output_data_block->Finalize();
            projection_table->Append(output_data_block);
        }

        outputs_[projection_table_index_] = projection_table;
    }
}

}
