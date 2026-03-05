module infinity_core:physical_show_indexes.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    std::vector<std::shared_ptr<TableIndexInfo>> table_index_info_list;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::vector<std::string> index_names;
    status = txn->ListIndex(db_name_, *object_name_, index_names);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
    }
    for (const auto &index_name : index_names) {
        auto [table_index_info, status] = txn->GetTableIndexInfo(db_name_, *object_name_, index_name);
        if (!status.ok()) {
            show_operator_state->status_ = status.clone();
            RecoverableError(status);
        }
        table_index_info_list.push_back(table_index_info);
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    {
        for (const auto &table_index_info : table_index_info_list) {
            if (output_block_ptr.get() == nullptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            size_t column_id = 0;
            {
                // Append index name to the first column
                Value value = Value::MakeVarchar(*table_index_info->index_name_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append index_comment to output
                std::string comment = *table_index_info->index_comment_;
                Value value = Value::MakeVarchar(comment);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append index method type to the second column
                Value value = Value::MakeVarchar(*table_index_info->index_type_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append index column id
                Value value = Value::MakeVarchar(*table_index_info->index_column_ids_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append index column names to the third column
                Value value = Value::MakeVarchar(*table_index_info->index_column_names_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append index path
                Value value = Value::MakeVarchar(*table_index_info->index_entry_dir_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append Index segment
                size_t segment_index_count = table_index_info->segment_index_count_;
                std::string result_value = fmt::format("{}", segment_index_count);
                Value value = Value::MakeVarchar(result_value);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }
            ++column_id;
            {
                // Append index other parameters to the fourth column
                Value value = Value::MakeVarchar(*table_index_info->index_other_params_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            if (++row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                show_operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}