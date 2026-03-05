module infinity_core:physical_show_columns.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowColumns(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    std::shared_ptr<TableInfo> table_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(table_info, status) = txn->GetTableInfo(db_name_, *object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    size_t column_count = table_info->column_count_;
    for (size_t input_column_id = 0; input_column_id < column_count; ++input_column_id) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        const ColumnDef *column = table_info->GetColumnDefByIdx(input_column_id);

        size_t output_column_idx = 0;
        {
            // Append column name to the 1st column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column type to the 2nd column, if the column type is embedded type, append the embedded type
            std::string column_type = column->type()->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column default value to the 3rd column
            std::string column_default = column->default_expr_->ToString();
            Value value = Value::MakeVarchar(column_default);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column comment to the 4th column
            Value value = Value::MakeVarchar(column->comment());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[output_column_idx]);
        }

        if (++row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            show_operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}