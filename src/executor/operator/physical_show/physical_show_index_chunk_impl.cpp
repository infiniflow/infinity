module infinity_core:physical_show_index_chunk.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :chunk_index_meta;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowIndexChunk(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    std::string base_name;
    RowID base_row_id;
    size_t row_cnt = 0;
    TxnTimeStamp deprecate_ts = 0;

    // Get tables from catalog
    NewTxn *txn = query_context->GetNewTxn();
    auto [chunk_index_info, status] = txn->GetChunkIndexInfo(db_name_, *object_name_, index_name_.value(), segment_id_.value(), chunk_id_.value());
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    base_name = chunk_index_info->base_name_;
    base_row_id = chunk_index_info->base_row_id_;
    row_cnt = chunk_index_info->row_cnt_;

    // Prepare the output data block
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    output_block_ptr->Init(*output_types_);

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("file_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(base_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("start_row");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(base_row_id.ToString());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(row_cnt));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("deprecate_timestamp");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(deprecate_ts));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}