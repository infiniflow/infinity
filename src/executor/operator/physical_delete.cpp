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

#include <string>

module physical_delete;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import query_context;
// import data_table;
import operator_state;
import logical_type;
import data_block;
import column_vector;
import internal_types;
import third_party;
import wal_manager;
import infinity_context;
import status;

namespace infinity {

void PhysicalDelete::Init() {}

bool PhysicalDelete::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to flush on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    OperatorState* prev_op_state = operator_state->prev_op_state_;

    SizeT data_block_count = prev_op_state->data_block_array_.size();
    for(SizeT block_idx = 0; block_idx < data_block_count; ++ block_idx) {
        DataBlock *input_data_block_ptr = prev_op_state->data_block_array_[block_idx].get();
        auto txn = query_context->GetTxn();
        Vector<RowID> row_ids;
        for (SizeT i = 0; i < input_data_block_ptr->column_count(); i++) {
            SharedPtr<ColumnVector> column_vector = input_data_block_ptr->column_vectors[i];
            if (column_vector->data_type()->type() == LogicalType::kRowID) {
                row_ids.resize(column_vector->Size());
                std::memcpy(row_ids.data(), column_vector->data(), column_vector->Size() * sizeof(RowID));
                break;
            }
        }
        if (!row_ids.empty()) {
            LOG_TRACE(fmt::format("Found to delete: row_count {}", row_ids.size()));
            txn->Delete(table_entry_ptr_, row_ids); // TODO: segment id in `row_ids` is fixed.
            DeleteOperatorState* delete_operator_state = static_cast<DeleteOperatorState*>(operator_state);
            ++ delete_operator_state->count_;
            delete_operator_state->sum_ += row_ids.size();
        } else {
            LOG_TRACE("DELETE: No row_id, skip");
        }
    }
    prev_op_state->data_block_array_.clear();
    if (prev_op_state->Complete()) {
        operator_state->SetComplete();
    }
    return true;
}

} // namespace infinity
