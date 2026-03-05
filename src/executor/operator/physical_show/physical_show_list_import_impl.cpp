module infinity_core:physical_show_list_import.impl;

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

void PhysicalShow::ExecuteListImport(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    NewTxnManager *txn_manager = query_context->storage()->new_txn_manager();
    std::vector<std::shared_ptr<TxnImportInfo>> txn_import_info_list = txn_manager->GetImportInfoList();

    output_block_ptr->Init(*output_types_);

    size_t row_count = 0;
    for (auto &txn_import_info : txn_import_info_list) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // txn
            Value value = Value::MakeBigInt(txn_import_info->txn_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // begin ts
            Value value = Value::MakeBigInt(txn_import_info->begin_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // commit ts
            Value value = Value::MakeBigInt(txn_import_info->commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // committed
            Value value = Value::MakeBool(txn_import_info->committed_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // detail
            std::string detail = fmt::format("{}({}).{}({}), segments: [{}], row: {}",
                                             txn_import_info->db_name_,
                                             txn_import_info->db_id_,
                                             txn_import_info->table_name_,
                                             txn_import_info->txn_id_,
                                             fmt::join(txn_import_info->segment_ids_, ","),
                                             txn_import_info->row_count_);
            Value value = Value::MakeVarchar(detail);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
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