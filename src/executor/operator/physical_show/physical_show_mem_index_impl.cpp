module infinity_core:physical_show_mem_index.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :query_context;
import :operator_state;
import :data_block;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowMemIndex(QueryContext *query_context, ShowOperatorState *operator_state) {

    Status status = Status::NotSupport("Show memindex is not supported in new catalog since BGMemIndexTracer has not yet been ported.");
    RecoverableError(status);

    //    auto varchar_type = std::make_shared<DataType>(LogicalType::kVarchar);
    //    auto bigint_type = std::make_shared<DataType>(LogicalType::kBigInt);
    //
    //    std::vector<std::shared_ptr<DataType>> column_types{
    //        varchar_type,
    //        varchar_type,
    //        varchar_type,
    //        bigint_type,
    //        bigint_type,
    //    };
    //    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    //    output_block_ptr->Init(*output_types_);
    //    size_t row_count = 0;

    //
    //    BGMemIndexTracer *mem_index_tracer = query_context->storage()->memindex_tracer();
    //    Txn *txn = query_context->GetTxn();
    //    std::vector<MemIndexTracerInfo> mem_index_tracer_info_array = mem_index_tracer->GetMemIndexTracerInfo(txn);
    //    for (const auto &memindex_tracer_info : mem_index_tracer_info_array) {
    //        if (output_block_ptr.get() == nullptr) {
    //            output_block_ptr = DataBlock::MakeUniquePtr();
    //            output_block_ptr->Init(*output_types_);
    //        }
    //
    //        {
    //            // index_name
    //            Value value = Value::MakeVarchar(*memindex_tracer_info.index_name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
    //        }
    //        {
    //            // table_name
    //            Value value = Value::MakeVarchar(*memindex_tracer_info.table_name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
    //        }
    //        {
    //            // db_name
    //            Value value = Value::MakeVarchar(*memindex_tracer_info.db_name_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
    //        }
    //        {
    //            // size
    //            Value value = Value::MakeBigInt(memindex_tracer_info.mem_used_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[3]);
    //        }
    //        {
    //            // row_count
    //            Value value = Value::MakeBigInt(memindex_tracer_info.row_count_);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[4]);
    //        }
    //
    //        ++row_count;
    //        if (row_count == output_block_ptr->capacity()) {
    //            output_block_ptr->Finalize();
    //            operator_state->output_.emplace_back(std::move(output_block_ptr));
    //            output_block_ptr = nullptr;
    //            row_count = 0;
    //        }
    //    }
    //    output_block_ptr->Finalize();
    //    operator_state->output_.emplace_back(std::move(output_block_ptr));
    //    return;
}