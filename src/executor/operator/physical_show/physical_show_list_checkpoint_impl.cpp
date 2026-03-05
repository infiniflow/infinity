module infinity_core:physical_show_list_checkpoint.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :new_txn_manager;
import :txn_info;

import std;

using namespace infinity;

void PhysicalShow::ExecuteListCheckpoint(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    NewTxnManager *txn_manager = query_context->storage()->new_txn_manager();
    std::vector<std::shared_ptr<TxnCheckpointInfo>> txn_checkpoint_info_list = txn_manager->GetCheckpointInfoList();

    output_block_ptr->Init(*output_types_);

    size_t row_count = 0;
    for (auto &txn_checkpoint_info : txn_checkpoint_info_list) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        if (!show_nullable_ && txn_checkpoint_info->entries_.empty()) {
            // Don't show null item
            continue;
        }

        {
            // txn
            Value value = Value::MakeBigInt(txn_checkpoint_info->txn_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // begin ts
            Value value = Value::MakeBigInt(txn_checkpoint_info->begin_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // commit ts
            Value value = Value::MakeBigInt(txn_checkpoint_info->commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // committed
            Value value = Value::MakeBool(txn_checkpoint_info->committed_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // checkpoint ts
            Value value = Value::MakeBigInt(txn_checkpoint_info->checkpoint_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        {
            // checkpoint type
            Value value = Value::MakeVarchar(txn_checkpoint_info->auto_flush_ ? "auto" : "manual");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[5]);
        }

        {
            // detail
            if (txn_checkpoint_info->entries_.empty()) {
                Value value = Value::MakeVarchar("null");
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[6]);
            } else {
                Value value = Value::MakeVarchar(fmt::format("entries: {}", txn_checkpoint_info->entries_.size()));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[6]);
            }
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}