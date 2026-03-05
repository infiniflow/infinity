module infinity_core:physical_show_transaction.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :txn_info;
import :new_txn_manager;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowTransaction(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<TxnInfo> txn_info = nullptr;
    NewTxnManager *new_txn_manager = query_context->storage()->new_txn_manager();
    txn_info = new_txn_manager->GetTxnInfoByID(*txn_id_);

    if (txn_info.get() == nullptr) {
        Status status = Status::TransactionNotFound(*txn_id_);
        RecoverableError(status);
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("transaction_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(*txn_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("session_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(*session_id_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("transaction_text");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            std::string txn_string;
            if (txn_info->txn_text_.get() != nullptr) {
                txn_string = *txn_info->txn_text_;
            }
            Value value = Value::MakeVarchar(txn_string);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}