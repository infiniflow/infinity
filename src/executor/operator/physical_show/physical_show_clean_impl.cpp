module infinity_core:physical_show_clean.impl;

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
import third_party;

using namespace infinity;

void PhysicalShow::ExecuteShowClean(QueryContext *query_context, ShowOperatorState *operator_state) {

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    NewTxnManager *txn_manager = query_context->storage()->new_txn_manager();
    std::vector<std::shared_ptr<TxnCleanInfo>> txn_clean_info_list = txn_manager->GetCleanInfoList();
    output_block_ptr->Init(*output_types_);
    for (auto &txn_clean_info : txn_clean_info_list) {
        if (txn_clean_info->txn_id_ == txn_id_) {
            // detail
            nlohmann::json clean_info_json;
            clean_info_json["txn_id"] = txn_clean_info->txn_id_;
            clean_info_json["begin_ts"] = txn_clean_info->begin_ts_;
            clean_info_json["commit_ts"] = txn_clean_info->commit_ts_;
            clean_info_json["committed"] = txn_clean_info->committed_;
            for (const auto &dropped_key : txn_clean_info->dropped_keys_) {
                clean_info_json["dropped_keys"].push_back(dropped_key);
            }

            for (const auto &meta_key : txn_clean_info->metas_) {
                clean_info_json["meta_key"].push_back(meta_key->ToJson());
            }

            auto meta_str = clean_info_json.dump(4);

            Value value = Value::MakeVarchar(meta_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            break;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}