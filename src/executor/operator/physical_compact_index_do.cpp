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

module physical_compact_index_do;

import stl;
import operator_state;
import txn;
import query_context;
import table_index_entry;
import block_index;
import status;
import base_table_ref;

namespace infinity {

bool PhysicalCompactIndexDo::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *compact_index_do_operator_state = static_cast<CompactIndexDoOperatorState *>(operator_state);
    auto *compact_state_data = compact_index_do_operator_state->compact_state_data_.get();
    BaseTableRef *new_table_ref = compact_state_data->GetNewTableRef();
    if (new_table_ref->block_index_->IsEmpty()) { // compact yield no new segment
        operator_state->SetComplete();
        return true;
    }

    auto *index_index = base_table_ref_->index_index_.get();
    SizeT create_index_idx = compact_index_do_operator_state->create_index_idx_;
    if (create_index_idx == index_index->index_snapshots_vec_.size()) {
        operator_state->SetComplete();
        return true;
    }
    auto *table_index_entry = index_index->index_snapshots_vec_[create_index_idx]->table_index_entry_;
    const String &index_name = *table_index_entry->GetIndexName();


    Txn *txn = query_context->GetTxn();
    auto &create_index_idxes = (*compact_index_do_operator_state->create_index_shared_data_)[create_index_idx]->create_index_idxes_;
    auto status = txn->CreateIndexDo(new_table_ref, index_name, create_index_idxes);

    ++create_index_idx;
    compact_index_do_operator_state->create_index_idx_ = create_index_idx;
    if (create_index_idx == index_index->index_snapshots_vec_.size()) {
        compact_index_do_operator_state->SetComplete();
    }
    return true;
}

} // namespace infinity