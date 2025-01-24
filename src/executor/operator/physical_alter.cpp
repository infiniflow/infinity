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

module physical_alter;

import query_context;
import operator_state;
import third_party;
import txn;
import status;
import infinity_exception;
import value;
import defer_op;
import wal_manager;
import infinity_context;

namespace infinity {

void PhysicalRenameTable::Init(QueryContext* query_context) {}

bool PhysicalRenameTable::Execute(QueryContext *query_context, OperatorState *operator_state) {
    RecoverableError(Status::NotSupport("Rename table is not supported."));
    operator_state->SetComplete();
    return true;
}

void PhysicalAddColumns::Init(QueryContext* query_context) {}

bool PhysicalAddColumns::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    Txn *txn = query_context->GetTxn();
    txn->LockTable(*table_info_->db_name_, *table_info_->table_name_);
    DeferFn defer_fn([&]() {
        txn->UnLockTable(*table_info_->db_name_, *table_info_->table_name_);
    });

    auto status = txn->AddColumns(*table_info_->db_name_, *table_info_->table_name_, column_defs_);
    if (!status.ok()) {
        RecoverableError(status);
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalDropColumns::Init(QueryContext* query_context) {}

bool PhysicalDropColumns::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    Txn *txn = query_context->GetTxn();
    txn->LockTable(*table_info_->db_name_, *table_info_->table_name_);
    DeferFn defer_fn([&]() {
        txn->UnLockTable(*table_info_->db_name_, *table_info_->table_name_);
    });

    auto status = txn->DropColumns(*table_info_->db_name_, *table_info_->table_name_, column_names_);
    if (!status.ok()) {
        RecoverableError(status);
    }

    operator_state->SetComplete();
    return true;
}

} // namespace infinity
