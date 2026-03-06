module infinity_core:physical_show_databases.impl;

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

void PhysicalShow::ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Get tables from catalog
    std::vector<DatabaseDetail> databases_detail;
    NewTxn *txn = query_context->GetNewTxn();
    std::vector<std::string> db_names;
    Status status = txn->ListDatabase(db_names);
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }
    for (auto &db_name : db_names) {
        auto [database_info, status] = txn->GetDatabaseInfo(db_name);
        if (!status.ok()) {
            RecoverableError(status);
            return;
        }
        databases_detail.emplace_back(DatabaseDetail{
            .db_name_ = std::make_shared<std::string>(db_name),
            .db_entry_dir_ = std::move(database_info->db_entry_dir_),
            .db_comment_ = std::move(database_info->db_comment_),
        });
    }

    // Prepare the output data block
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (auto &database_detail : databases_detail) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        size_t column_id = 0;
        {
            // Append schema name to the 0 column
            const std::string *db_name = database_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            // Append entry dir to the 1 column
            const std::string *db_entry_dir = database_detail.db_entry_dir_.get();
            Value value = Value::MakeVarchar(*db_entry_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            // Append comment to the 2 column
            const std::string *db_comment = database_detail.db_comment_.get();
            Value value = Value::MakeVarchar(*db_comment);
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

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}