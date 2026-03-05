module infinity_core:physical_show_tables.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowTables(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema

    // Get tables from catalog
    std::vector<std::shared_ptr<TableDetail>> tables_detail;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    status = txn->GetTables(db_name_, tables_detail);
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (auto &table_detail_ptr : tables_detail) {
        // Initialize the output data block
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        size_t column_id = 0;
        {
            const std::string *db_name = table_detail_ptr->db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            const std::string *table_name = table_detail_ptr->table_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            const std::string *table_id = table_detail_ptr->table_id_.get();
            Value value = Value::MakeVarchar(*table_id);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->segment_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->block_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(static_cast<i64>(table_detail_ptr->create_ts_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_detail_ptr->table_comment_);
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