module infinity_core:physical_show_list_catalog_key.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :kv_store;
import :utility;

import std;

using namespace infinity;

void PhysicalShow::ExecuteListCatalogKey(QueryContext *query_context, ShowOperatorState *operator_state) {

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    NewTxn *txn = query_context->GetNewTxn();
    KVInstance *kv_instance = txn->kv_instance();
    auto all_key_values = kv_instance->GetAllKeyValue();

    if (*object_name_ == "all") {
        for (const auto &meta_pair : all_key_values) {
            if (output_block_ptr.get() == nullptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            {
                // key
                Value value = Value::MakeVarchar(meta_pair.first);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            }
            {
                // value
                Value value = Value::MakeVarchar(meta_pair.second);
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

    } else {

        const std::string &key_name = object_name_.value();
        for (const auto &meta_pair : all_key_values) {
            auto keys = infinity::Partition(meta_pair.first, '|');
            if (keys[0] == key_name) {
                if (output_block_ptr.get() == nullptr) {
                    output_block_ptr = DataBlock::MakeUniquePtr();
                    output_block_ptr->Init(*output_types_);
                }

                {
                    // key
                    Value value = Value::MakeVarchar(meta_pair.first);
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
                }
                {
                    // value
                    Value value = Value::MakeVarchar(meta_pair.second);
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
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}