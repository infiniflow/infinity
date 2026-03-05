module infinity_core:physical_show_buffer.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowBuffer(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    BufferManager *buffer_manager = query_context->storage()->buffer_manager();
    std::vector<BufferObjectInfo> buffer_object_info_array = buffer_manager->GetBufferObjectsInfo();
    for (const auto &buffer_object_info : buffer_object_info_array) {

        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // path
            Value value = Value::MakeVarchar(buffer_object_info.object_path_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // status
            Value value = Value::MakeVarchar(BufferStatusToString(buffer_object_info.buffered_status_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // size
            i64 buffer_object_size = static_cast<i64>(buffer_object_info.object_size_);
            Value value = Value::MakeBigInt(buffer_object_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
        {
            // buffered type
            Value value = Value::MakeVarchar(BufferTypeToString(buffer_object_info.buffered_type_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }
        {
            // type
            Value value = Value::MakeVarchar(FileWorkerType2Str(buffer_object_info.file_type_));
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