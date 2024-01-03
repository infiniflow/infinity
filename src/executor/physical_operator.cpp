// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import physical_operator_type;
import default_values;
import operator_state;
import column_buffer;
import column_vector;
import query_context;
import parser;
import base_table_ref;
import third_party;
import infinity_exception;
import catalog;

#include <algorithm>

module physical_operator;

namespace infinity {

String PhysicalOperator::GetName() const { return PhysicalOperatorToString(operator_type_); }

void PhysicalOperator::InputLoad(QueryContext *query_context, OperatorState *operator_state, HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) {
    if (load_metas_.get() == nullptr || load_metas_->empty()) {
        return;
    }
    auto load_metas = *load_metas_.get();
    // FIXME: After columnar reading is supported, use a different table_ref for each LoadMetas
    auto table_ref = table_refs[load_metas[0].binding_.table_idx];
    if (table_ref.get() == nullptr) {
        Error<ExecutorException>("TableRef not found!");
    }

    for (SizeT i = 0; i < operator_state->prev_op_state_->data_block_array_.size(); ++i) {
        auto input_block = operator_state->prev_op_state_->data_block_array_[i].get();
        SizeT load_column_count = load_metas_->size();

        u16 row_count = input_block->row_count();
        SizeT capacity = input_block->capacity();

        // Filling ColumnVector
        for (SizeT j = 0; j < load_column_count; ++j) {
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(load_metas[j].type_);
            column_vector->Initialize(ColumnVectorType::kFlat, capacity);

            input_block->InsertVector(column_vector, load_metas[j].index_);
        }

        auto row_column_id = input_block->column_count() - 1;

        for (SizeT j = 0; j < row_count; ++j) {
            // If late materialization needs to be optional, then this needs to be modified
            RowID row_id = input_block->GetValue(row_column_id, j).value_.row;
            u32 segment_id = row_id.segment_id_;
            u32 segment_offset = row_id.segment_offset_;
            u16 block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            u16 block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

            const BlockEntry* block_entry = table_ref->table_entry_ptr_->GetBlockEntryByID(segment_id, block_id);
            for (SizeT k = 0; k < load_column_count; ++k) {
                auto binding = load_metas[k].binding_;
                BlockColumnEntry* block_column_ptr = block_entry->GetColumnBlockEntry(binding.column_idx);
                ColumnBuffer column_buffer = block_column_ptr->GetColumnData(query_context->storage()->buffer_manager());
                input_block->column_vectors[load_metas[k].index_]->AppendWith(column_buffer, block_offset, 1);
            }
        }
    }
}

} // namespace infinity
