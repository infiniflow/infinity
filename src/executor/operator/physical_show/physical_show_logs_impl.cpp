module infinity_core:physical_show_logs.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowLogs(QueryContext *query_context, ShowOperatorState *operator_state) {

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    WalManager *wal_manager = query_context->storage()->wal_manager();
    std::vector<std::shared_ptr<WalEntry>> wal_entries = wal_manager->CollectWalEntries();
    LOG_TRACE(fmt::format("WAL Entries count: {}", wal_entries.size()));

    for (const auto &wal_entry_ref : wal_entries) {
        for (const auto &cmd_ref : wal_entry_ref->cmds_) {
            if (output_block_ptr.get() == nullptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            {
                // transaction_id
                Value value = Value::MakeBigInt(wal_entry_ref->txn_id_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            }

            {
                // commit_ts
                Value value = Value::MakeBigInt(wal_entry_ref->commit_ts_);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            }

            {
                // command_type
                std::string command_type = WalCmd::WalCommandTypeToString(cmd_ref->GetType());
                Value value = Value::MakeVarchar(command_type);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
            }

            {
                // command_text
                std::string command_text = cmd_ref->ToString();
                Value value = Value::MakeVarchar(command_text);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
            }

            ++row_count;
            if (row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}