module infinity_core:physical_show_memory_objects.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowMemoryObjects(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    std::unordered_map<std::string, i64> object_map = GlobalResourceUsage::GetObjectClones();
    size_t row_count = 0;

    for (auto &object_pair : object_map) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }
        if (object_pair.second != 0) {

            {
                // object_name
                Value value = Value::MakeVarchar(object_pair.first);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
            }
            {
                // object_count
                Value value = Value::MakeBigInt(object_pair.second);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
            }

            ++row_count;
        }
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