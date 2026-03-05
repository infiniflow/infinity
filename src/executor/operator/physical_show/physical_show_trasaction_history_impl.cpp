module infinity_core:physical_show_trasaction_history.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :new_txn_manager;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowTransactionHistory(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::vector<std::shared_ptr<TxnContext>> txn_context_histories;
    TransactionID this_txn_id;
    NewTxnManager *new_txn_manager = query_context->storage()->new_txn_manager();
    txn_context_histories = new_txn_manager->GetTxnContextHistories();
    NewTxn *txn = query_context->GetNewTxn();
    this_txn_id = txn->TxnID();

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    for (const auto &txn_context : txn_context_histories) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // txn id
            std::string txn_id_str;
            if (this_txn_id == txn_context->txn_id_) {
                txn_id_str = fmt::format("{}(this txn)", this_txn_id);
            } else {
                txn_id_str = fmt::format("{}", txn_context->txn_id_);
            }
            Value value = Value::MakeVarchar(txn_id_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // txn id
            std::string txn_text;
            if (txn_context->text_.get() != nullptr) {
                txn_text = *txn_context->text_;
            }
            Value value = Value::MakeVarchar(txn_text);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // txn begin_ts
            Value value = Value::MakeBigInt(txn_context->begin_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // txn commit_ts
            Value value = Value::MakeBigInt(txn_context->commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // txn state
            Value value = Value::MakeVarchar(TxnState2Str(txn_context->state_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        {
            // txn type
            std::string transaction_type_str = "read";
            if (txn_context->is_write_transaction_) {
                transaction_type_str = "write";
            }
            Value value = Value::MakeVarchar(transaction_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[5]);
        }

        {
            // txn operations
            std::stringstream ss;
            std::vector<std::shared_ptr<std::string>> operations;
            for (const auto &ops : txn_context->operations_) {
                ss << *ops << std::endl;
            }
            Value value = Value::MakeVarchar(ss.str());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[6]);
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