module infinity_core:physical_show_index_chunks.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :chunk_index_meta;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowIndexChunks(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    NewTxn *txn = query_context->GetNewTxn();
    auto [chunk_index_infos, status] = txn->GetChunkIndexesInfo(db_name_, *object_name_, index_name_.value(), segment_id_.value());
    if (!status.ok()) {
        RecoverableError(status);
        return;
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    for (auto &chunk_index_info : chunk_index_infos) {
        size_t column_id = 0;
        {
            Value value = Value::MakeBigInt(chunk_index_info.first);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(chunk_index_info.second->base_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(chunk_index_info.second->row_cnt_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}