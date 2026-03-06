module infinity_core:physical_show_database.impl;

import :physical_show;
import :new_txn;
import :value;
import :status;
import :value_expression;
import :query_context;
import :operator_state;
import :data_block;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowDatabase(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Get tables from catalog
    std::shared_ptr<DatabaseInfo> database_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(database_info, status) = txn->GetDatabaseInfo(db_name_);

    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    output_block_ptr->Init(*output_types_);

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            // Append database name to the 1 column
            const std::string *table_name = database_info->db_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            // Append database storage directory to the 1 column
            Value value = Value::MakeVarchar(database_info->absolute_db_path_ ? *database_info->absolute_db_path_ : "N/A");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("table_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            // Append database storage directory to the 1 column
            Value value = Value::MakeVarchar(std::to_string(database_info->table_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("comment");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*(database_info->db_comment_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}