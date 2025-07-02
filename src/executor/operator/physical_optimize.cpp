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

#include <tuple>

module physical_optimize;

import stl;
import query_context;

import operator_state;
import infinity_exception;
import table_def;
import third_party;
import status;
import logger;
import base_table_ref;

import wal_manager;
import infinity_context;
import new_txn;

namespace infinity {

void PhysicalOptimize::Init(QueryContext *query_context) {}

bool PhysicalOptimize::Execute(QueryContext *query_context, OperatorState *operator_state) {
    StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
    if (storage_mode == StorageMode::kUnInitialized) {
        UnrecoverableError("Uninitialized storage mode");
    }

    if (storage_mode != StorageMode::kWritable) {
        operator_state->status_ = Status::InvalidNodeRole("Attempt to write on non-writable node");
        operator_state->SetComplete();
        return true;
    }

    if (index_name_.empty()) {
        OptimizeIndex(query_context, operator_state);
    } else {
        OptIndex(query_context, operator_state);
    }

    operator_state->SetComplete();
    return true;
}

void PhysicalOptimize::OptimizeIndex(QueryContext *query_context, OperatorState *operator_state) {
    // Get tables from catalog
    LOG_INFO(fmt::format("OptimizeIndex {}.{} begin", db_name_, table_name_));

    NewTxn *new_txn = query_context->GetNewTxn();
    Status status = new_txn->OptimizeTableIndexes(db_name_, table_name_);
    if (!status.ok()) {
        operator_state->status_ = status;
        RecoverableError(status);
    }
}

void PhysicalOptimize::OptIndex(QueryContext *query_context, OperatorState *operator_state) {
    LOG_INFO(fmt::format("OptimizeIndex {}.{}::{} begin", db_name_, table_name_, index_name_));

    NewTxn *new_txn = query_context->GetNewTxn();
    Status status = new_txn->OptimizeIndexByParams(db_name_, table_name_, index_name_, std::move(opt_params_));
    if (!status.ok()) {
        operator_state->status_ = status;
        RecoverableError(status);
    }
}

} // namespace infinity
