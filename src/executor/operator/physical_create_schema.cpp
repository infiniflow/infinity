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

module physical_create_schema;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import physical_operator_type;
import operator_state;
import wal_manager;
import infinity_context;
import status;
import new_txn;

namespace infinity {

void PhysicalCreateSchema::Init(QueryContext *query_context) {}

bool PhysicalCreateSchema::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    bool use_new_catalog = query_context->global_config()->UseNewCatalog();
    if (use_new_catalog) {
        NewTxn *new_txn = query_context->GetNewTxn();
        Status status = new_txn->CreateDatabase(*schema_name_, conflict_type_, comment_);
        if (!status.ok()) {
            operator_state->status_ = status;
        }
    } else {
        auto txn = query_context->GetTxn();
        Status status = txn->CreateDatabase(schema_name_, conflict_type_, comment_);
        if (!status.ok()) {
            operator_state->status_ = status;
        }
    }

    operator_state->SetComplete();
    return true;
}

} // namespace infinity
