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
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import status;
import infinity_exception;
import catalog;

module physical_create_index;

namespace infinity {

void PhysicalCreateIndex::Init() {}

bool PhysicalCreateIndex::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *txn = query_context->GetTxn();
    TableIndexEntry *table_index_entry = nullptr;
    Status status = txn->CreateIndexDef(base_table_ref_->table_entry_ptr_, index_def_ptr_, conflict_type_, table_index_entry);
    if (!status.ok() || table_index_entry == nullptr) {
        operator_state->status_ = status;
    } else {
        txn->CreateIndexPrepare(table_index_entry, base_table_ref_.get(), false);
    }
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
