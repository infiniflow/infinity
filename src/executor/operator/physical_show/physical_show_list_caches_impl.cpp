module infinity_core:physical_show_list_caches.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :meta_cache;

import std;

using namespace infinity;

void PhysicalShow::ExecuteListCaches(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Storage *storage = query_context->storage();
    MetaCache *meta_cache = storage->meta_cache();
    std::vector<std::shared_ptr<MetaBaseCache>> cache_items = meta_cache->GetAllCacheItems();

    output_block_ptr->Init(*output_types_);

    size_t row_count = 0;
    for (auto &cache_item : cache_items) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // name
            Value value = Value::MakeVarchar(cache_item->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // cache type
            Value value = Value::MakeVarchar(ToString(cache_item->type()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // commit ts
            Value value = Value::MakeBigInt(cache_item->commit_ts());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // detailed info
            Value value = Value::MakeVarchar(cache_item->detail());
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

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}