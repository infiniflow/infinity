module infinity_core:physical_show_queries.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :session_manager;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowQueries(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    SessionManager *session_manager = query_context->session_manager();
    std::map<u64, std::shared_ptr<QueryInfo>> query_map = session_manager->QueryRecords();
    for (auto &query_pair : query_map) {
        u64 session_id = query_pair.first;
        QueryInfo &query_info = *query_pair.second;

        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // session_id
            Value value = Value::MakeBigInt(session_id);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // query_id
            Value value = Value::MakeBigInt(query_info.query_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // query_kind
            Value value = Value::MakeVarchar(query_info.query_kind_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
        {
            // start_time
            Value value = Value::MakeVarchar(query_info.profiler_.BeginTime());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }
        {
            // time_consumption
            Value value = Value::MakeVarchar(query_info.profiler_.ElapsedToString());
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