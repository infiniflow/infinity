module infinity_core:physical_show_block_detail.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :infinity_context;
import :utility;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowBlockDetail(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    std::shared_ptr<BlockInfo> block_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(block_info, status) = txn->GetBlockInfo(db_name_, *object_name_, *segment_id_, *block_id_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(block_info->block_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            std::string full_block_dir = std::filesystem::path(InfinityContext::instance().config()->DataDir()) / *block_info->block_dir_;
            Value value = Value::MakeVarchar(full_block_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            size_t block_storage_size = block_info->storage_size_;
            std::string block_storage_size_str = Utility::FormatByteSize(block_storage_size);
            Value value = Value::MakeVarchar(block_storage_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("row_capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            size_t row_capacity = block_info->row_capacity_;
            Value value = Value::MakeVarchar(std::to_string(row_capacity));
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
            size_t row_count = block_info->row_count_;
            Value value = Value::MakeVarchar(std::to_string(row_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("checkpoint_row_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            size_t checkpoint_row_count = block_info->checkpoint_row_count_;
            Value value = Value::MakeVarchar(std::to_string(checkpoint_row_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("column_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            size_t column_count = block_info->column_count_;
            Value value = Value::MakeVarchar(std::to_string(column_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("checkpoint_ts");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            size_t checkpoint_ts = block_info->checkpoint_ts_;
            Value value = Value::MakeVarchar(std::to_string(checkpoint_ts));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}
