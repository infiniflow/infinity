module infinity_core:physical_show_persistence_objects.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowPersistenceObjects(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    PersistenceManager *persistence_manager = query_context->persistence_manager();
    if (persistence_manager == nullptr) {
        Status status = Status::FunctionIsDisable("Persistence");
        RecoverableError(status);
    }

    std::unordered_map<std::string, std::shared_ptr<ObjStat>> object_map = persistence_manager->GetAllObjects();
    for (const auto &object_pair : object_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // name
            Value value = Value::MakeVarchar(object_pair.first);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // reference count
            Value value = Value::MakeBigInt(object_pair.second->ref_count_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }
        {
            // size
            Value value = Value::MakeBigInt(object_pair.second->obj_size_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }
        {
            // parts
            Value value = Value::MakeBigInt(object_pair.second->parts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }
        {
            // deleted ranges
            std::ostringstream oss;
            for (const auto &range : object_pair.second->deleted_ranges_) {
                oss << fmt::format("[{}, {}) ", range.start_, range.end_);
            }
            std::string deleted_ranges = oss.str();
            Value value = Value::MakeVarchar(deleted_ranges);
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