//
// Created by JinHai on 2022/7/28.
//

#include "storage/table_with_fix_row.h"
#include "physical_insert.h"
#include "expression/value_expression.h"
#include "common/utility/infinity_assert.h"
#include "executor/expression/expression_state.h"
#include "executor/expression/expression_evaluator.h"

namespace infinity {

void
PhysicalInsert::Init() {

}

#if 0
void
PhysicalInsert::Execute(QueryContext* query_context) {

    // TODO: execute insert into table;
    Vector<DataType> output_types;
    SizeT value_count = value_list_.size();
    for(SizeT idx = 0; idx < value_count; ++ idx) {
        if (value_list_[idx]->type() != ExpressionType::kValue) {
            ExecutorError("Not value expression.");
        }
        output_types.emplace_back(value_list_[idx]->Type());
    }

    SharedPtr<DataBlock> output_block = MakeShared<DataBlock>();
    output_block->Init(output_types);
    for(SizeT idx = 0; idx < value_count; ++ idx) {
        SharedPtr<ValueExpression> value_expr_ptr = std::static_pointer_cast<ValueExpression>(value_list_[idx]);
        value_expr_ptr->AppendToChunk(output_block->column_vectors[idx]);
    }
    // Finalize the output block row count.
    output_block->Finalize();

    table_ptr_->Append(output_block);

    // Generate the result table
    Vector<SharedPtr<ColumnDef>> column_defs;
    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(table_def_ptr, TableType::kResult);
}
#else

void
PhysicalInsert::Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) {

}

void
PhysicalInsert::Execute(QueryContext* query_context) {
    SizeT column_count = value_list_.size();
    SizeT table_collection_column_count = table_collection_entry_->columns_.size();
    if(column_count != table_collection_column_count) {
        ExecutorError(fmt::format("Insert values count{} isn't matched with table column count{}.",
                                  column_count,
                                  table_collection_column_count));
    }

    // Prepare the expression states
    Vector<SharedPtr<ExpressionState>> expr_states;
    expr_states.reserve(column_count);

    // Prepare the output block
    Vector<SharedPtr<DataType>> output_types;
    output_types.reserve(column_count);

    for(auto& expr: value_list_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(expr));
        // for output block
        output_types.emplace_back(MakeShared<DataType>(expr->Type()));
    }

    SharedPtr<DataBlock> output_block = DataBlock::Make();
    output_block->Init(output_types);

    ExpressionEvaluator evaluator;
    evaluator.Init(nullptr);
    for(SizeT expr_idx = 0; expr_idx < column_count; ++expr_idx) {
        Vector<SharedPtr<ColumnVector>> blocks_column;
        blocks_column.emplace_back(output_block->column_vectors[expr_idx]);
        evaluator.Execute(value_list_[expr_idx],
                          expr_states[expr_idx],
                          output_block->column_vectors[expr_idx]);
        if(blocks_column[0].get() != output_block->column_vectors[expr_idx].get()) {
            ExecutorError("Unexpected error");
        }
    }

    output_block->Finalize();

    // FIXME: to implemented
#if 0
    SizeT block_count = table_ptr_->DataBlockCount();
    if(block_count == 0) {
        table_ptr_->Append(output_block);
    } else {
        SizeT last_block_id = block_count - 1;
        SharedPtr<DataBlock>& last_block = table_ptr_->GetDataBlockById(last_block_id);
        if(last_block->row_count() + output_block->row_count() <= last_block->capacity()) {
            last_block->AppendWith(output_block);
            last_block->Finalize();
            table_ptr_->UpdateRowCount(output_block->row_count());
        } else {
            table_ptr_->Append(output_block);
        }
    }
#endif

    // Generate the result table
    Vector<SharedPtr<ColumnDef>> column_defs;
    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<Table>(table_def_ptr, TableType::kResult);
}
#endif
}

