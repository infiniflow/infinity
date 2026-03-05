module infinity_core:physical_show_cache.impl;

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

void PhysicalShow::ExecuteShowCache(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Storage *storage = query_context->storage();
    MetaCache *meta_cache = storage->meta_cache();
    std::vector<std::shared_ptr<MetaBaseCache>> cache_items = meta_cache->GetAllCacheItems();

    output_block_ptr->Init(*output_types_);

    {
        // db_cache
        CacheStatus db_cache_status = meta_cache->GetCacheStatus(MetaCacheType::kCreateDB);
        {
            // cache_type
            Value value = Value::MakeVarchar("database");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // db cache number
            Value value = Value::MakeBigInt(db_cache_status.item_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // hit count
            Value value = Value::MakeBigInt(db_cache_status.hit_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // request count
            Value value = Value::MakeBigInt(db_cache_status.request_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // hit rate
            double hit_rate = 0;
            if (db_cache_status.request_count_ > 0) {
                hit_rate = (double)(db_cache_status.hit_count_) / (double)(db_cache_status.request_count_);
            }
            Value value = Value::MakeDouble(hit_rate);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }
    }

    {
        // table_cache
        CacheStatus table_cache_status = meta_cache->GetCacheStatus(MetaCacheType::kCreateTable);
        {
            // cache_type
            Value value = Value::MakeVarchar("table");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // db cache number
            Value value = Value::MakeBigInt(table_cache_status.item_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // hit count
            Value value = Value::MakeBigInt(table_cache_status.hit_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // request count
            Value value = Value::MakeBigInt(table_cache_status.request_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // hit rate
            double hit_rate = 0;
            if (table_cache_status.request_count_ > 0) {
                hit_rate = (double)(table_cache_status.hit_count_) / (double)(table_cache_status.request_count_);
            }
            Value value = Value::MakeDouble(hit_rate);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }
    }

    {
        // index_cache
        CacheStatus index_cache_status = meta_cache->GetCacheStatus(MetaCacheType::kCreateIndex);
        {
            // cache_type
            Value value = Value::MakeVarchar("index");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // db cache number
            Value value = Value::MakeBigInt(index_cache_status.item_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // hit count
            Value value = Value::MakeBigInt(index_cache_status.hit_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // request count
            Value value = Value::MakeBigInt(index_cache_status.request_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // hit rate
            double hit_rate = 0;
            if (index_cache_status.request_count_ > 0) {
                hit_rate = (double)(index_cache_status.hit_count_) / (double)(index_cache_status.request_count_);
            }

            Value value = Value::MakeDouble(hit_rate);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}