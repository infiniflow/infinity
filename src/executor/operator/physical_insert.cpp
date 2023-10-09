//
// Created by JinHai on 2022/7/28.
//

#include "physical_insert.h"
#include "common/utility/infinity_assert.h"
#include "executor/expression/expression_evaluator.h"
#include "executor/expression/expression_state.h"
#include "expression/value_expression.h"

namespace infinity {

void PhysicalInsert::Init() {}

void PhysicalInsert::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    SizeT column_count = value_list_.size();
    SizeT table_collection_column_count = table_collection_entry_->columns_.size();
    if (column_count != table_collection_column_count) {
        ExecutorError(fmt::format("Insert values count{} isn't matched with table column count{}.", column_count, table_collection_column_count));
    }

    // Prepare the expression states
    Vector<SharedPtr<ExpressionState>> expr_states;
    expr_states.reserve(column_count);

    // Prepare the output block
    Vector<SharedPtr<DataType>> output_types;
    output_types.reserve(column_count);

    for (auto &expr : value_list_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(expr));
        // for output block
        output_types.emplace_back(MakeShared<DataType>(expr->Type()));
    }

    SharedPtr<DataBlock> output_block = DataBlock::Make();
    output_block->Init(output_types);

    ExpressionEvaluator evaluator;
    evaluator.Init(nullptr);
    for (SizeT expr_idx = 0; expr_idx < column_count; ++expr_idx) {
        Vector<SharedPtr<ColumnVector>> blocks_column;
        blocks_column.emplace_back(output_block->column_vectors[expr_idx]);
        evaluator.Execute(value_list_[expr_idx], expr_states[expr_idx], output_block->column_vectors[expr_idx]);
        if (blocks_column[0].get() != output_block->column_vectors[expr_idx].get()) {
            ExecutorError("Unexpected error");
        }
    }

    output_block->Finalize();

    auto *txn = query_context->GetTxn();
    const String &db_name = *TableCollectionEntry::GetDBEntry(table_collection_entry_)->db_name_;
    const String &table_name = *table_collection_entry_->table_collection_name_;
    txn->Append(db_name, table_name, output_block);

    UniquePtr<String> result_msg = MakeUnique<String>(fmt::format("INSERTED {} Rows", output_block->row_count()));
    if (output_state == nullptr) {
        // Generate the result table
        Vector<SharedPtr<ColumnDef>> column_defs;
        SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
        output_ = MakeShared<Table>(result_table_def_ptr, TableType::kDataTable);
        output_->SetResultMsg(std::move(result_msg));
    } else {
        InsertOutputState *insert_output_state = static_cast<InsertOutputState *>(output_state);
        insert_output_state->result_msg_ = std::move(result_msg);
    }
    output_state->SetComplete();
}

void PhysicalInsert::Execute(QueryContext *query_context) { Execute(query_context, nullptr, nullptr); }

} // namespace infinity
