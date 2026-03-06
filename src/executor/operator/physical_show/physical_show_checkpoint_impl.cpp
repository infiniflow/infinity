module infinity_core:physical_show_checkpoint.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :new_txn_manager;
import :txn_info;
import :base_txn_store;

import std;
import third_party;

using namespace infinity;

void PhysicalShow::ExecuteShowCheckpoint(QueryContext *query_context, ShowOperatorState *operator_state) {

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    NewTxnManager *txn_manager = query_context->storage()->new_txn_manager();
    std::vector<std::shared_ptr<TxnCheckpointInfo>> txn_checkpoint_info_list = txn_manager->GetCheckpointInfoList();
    output_block_ptr->Init(*output_types_);
    for (auto &txn_checkpoint_info : txn_checkpoint_info_list) {
        if (txn_checkpoint_info->txn_id_ == txn_id_) {
            // detail
            nlohmann::json ckp_info_json;
            ckp_info_json["txn_id"] = txn_checkpoint_info->txn_id_;
            ckp_info_json["begin_ts"] = txn_checkpoint_info->begin_ts_;
            ckp_info_json["commit_ts"] = txn_checkpoint_info->commit_ts_;
            ckp_info_json["checkpoint_ts"] = txn_checkpoint_info->checkpoint_ts_;
            ckp_info_json["committed"] = txn_checkpoint_info->committed_;
            ckp_info_json["auto_flush"] = txn_checkpoint_info->auto_flush_;
            for (const std::shared_ptr<FlushDataEntry> &data_entry : txn_checkpoint_info->entries_) {
                nlohmann::json ckp_info_entry;
                ckp_info_entry["db_id"] = data_entry->db_id_str_;
                ckp_info_entry["table_id"] = data_entry->table_id_str_;
                ckp_info_entry["segment_id"] = data_entry->segment_id_;
                ckp_info_entry["block_id"] = data_entry->block_id_;
                ckp_info_entry["to_flush"] = data_entry->to_flush_;
                ckp_info_json["entries"].push_back(ckp_info_entry);
            }

            auto meta_str = ckp_info_json.dump(4);

            Value value = Value::MakeVarchar(meta_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}