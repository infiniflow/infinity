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

#include <vector>
module physical_operator;

import stl;
import physical_operator_type;
import default_values;
import operator_state;
import column_vector;
import query_context;

import base_table_ref;
import third_party;
import infinity_exception;
import block_entry;
import block_column_entry;
import logical_type;
import internal_types;

import logger;

namespace infinity {

String PhysicalOperator::GetName() const { return PhysicalOperatorToString(operator_type_); }

void PhysicalOperator::InputLoad(QueryContext *query_context, OperatorState *operator_state, HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) {
    if (load_metas_.get() == nullptr || load_metas_->empty()) {
        return;
    }
//    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();

    auto load_metas = *load_metas_.get();
    // FIXME: After columnar reading is supported, use a different table_ref for each LoadMetas
    auto table_ref = table_refs[load_metas[0].binding_.table_idx];
    if (table_ref.get() == nullptr) {
        String error_message = "TableRef not found";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }

    for (SizeT i = 0; i < operator_state->prev_op_state_->data_block_array_.size(); ++i) {
        auto input_block = operator_state->prev_op_state_->data_block_array_[i].get();
        SizeT load_column_count = load_metas_->size();

        u16 row_count = input_block->row_count();
        SizeT capacity = input_block->capacity();

        // Filling ColumnVector
        for (SizeT j = 0; j < load_column_count; ++j) {
            SharedPtr<ColumnVector> column_vector = ColumnVector::Make(load_metas[j].type_);
            auto column_vector_type =
                (load_metas[j].type_->type() == LogicalType::kBoolean) ? ColumnVectorType::kCompactBit : ColumnVectorType::kFlat;
            column_vector->Initialize(column_vector_type, capacity);

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

            const BlockEntry *block_entry = table_ref->block_index_->GetBlockEntry(segment_id, block_id);
            for (SizeT k = 0; k < load_column_count; ++k) {
                auto binding = load_metas[k].binding_;
                BlockColumnEntry *block_column_ptr = block_entry->GetColumnBlockEntry(binding.column_idx);

                ColumnVector column_vector = block_column_ptr->GetColumnVector(query_context->storage()->buffer_manager());
                input_block->column_vectors[load_metas[k].index_]->AppendWith(column_vector, block_offset, 1);
            }
        }
    }
}

SharedPtr<Vector<String>> PhysicalCommonFunctionUsingLoadMeta::GetOutputNames(const PhysicalOperator &op) {
    auto prev_output_names = op.left()->GetOutputNames();
    auto output_names = MakeShared<Vector<String>>(*prev_output_names);
    if (auto &ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_names->insert(output_names->begin() + load_meta.index_, load_meta.column_name_);
        }
    }
    return output_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalCommonFunctionUsingLoadMeta::GetOutputTypes(const PhysicalOperator &op) {
    auto prev_output_types = op.left()->GetOutputTypes();
    auto output_types = MakeShared<Vector<SharedPtr<DataType>>>(*prev_output_types);
    if (auto &ptr = op.load_metas(); ptr) {
        for (auto &load_meta : *ptr) {
            output_types->insert(output_types->begin() + load_meta.index_, load_meta.type_);
        }
    }
    return output_types;
}

} // namespace infinity
