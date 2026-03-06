module infinity_core:physical_show_persistence_object.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowPersistenceObject(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    PersistenceManager *persistence_manager = query_context->persistence_manager();
    if (persistence_manager == nullptr) {
        Status status = Status::FunctionIsDisable("Persistence");
        RecoverableError(status);
    }

    std::unordered_map<std::string, std::shared_ptr<ObjStat>> object_map = persistence_manager->GetAllObjects();
    auto iter = object_map.find(*object_name_);
    if (iter == object_map.end()) {
        Status status = Status::FileNotFound(*object_name_);
        RecoverableError(status);
    }

    std::set<Range> &deleted_ranges = iter->second->deleted_ranges_;
    for (auto &range : deleted_ranges) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }
        {
            // start
            Value value = Value::MakeBigInt(range.start_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }
        {
            // end
            Value value = Value::MakeBigInt(range.end_);
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

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}