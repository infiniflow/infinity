module infinity_core:physical_show_transactions.impl;

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

void PhysicalShow::ExecuteShowTransactions(QueryContext *query_context, ShowOperatorState *operator_state) {
    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    std::vector<TxnInfo> txn_info_array;
    NewTxnManager *new_txn_manager = query_context->storage()->new_txn_manager();
    txn_info_array = new_txn_manager->GetTxnInfoArray();

    for (const auto &txn_info : txn_info_array) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // txn_id
            Value value = Value::MakeBigInt(txn_info.txn_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // txn_text
            std::string txn_string;
            if (txn_info.txn_text_.get() != nullptr) {
                txn_string = *txn_info.txn_text_;
            }
            Value value = Value::MakeVarchar(txn_string);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
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