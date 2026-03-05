module infinity_core:physical_show_persistence_files.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :persistence_manager;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowPersistenceFiles(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    PersistenceManager *persistence_manager = query_context->persistence_manager();
    if (persistence_manager == nullptr) {
        Status status = Status::FunctionIsDisable("Persistence");
        RecoverableError(status);
    }
    std::unordered_map<std::string, ObjAddr> file_map = persistence_manager->GetAllFiles();

    for (const auto &file_pair : file_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // file_name
            Value value = Value::MakeVarchar(file_pair.first);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // object_name
            Value value = Value::MakeVarchar(file_pair.second.obj_key_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // file offset
            Value value = Value::MakeBigInt(file_pair.second.part_offset_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
        {
            // file size
            Value value = Value::MakeBigInt(file_pair.second.part_size_);
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