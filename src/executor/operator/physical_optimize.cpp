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
import txn;
import query_context;

import operator_state;
import infinity_exception;
import table_def;
import third_party;
import status;
import logger;
import base_table_ref;
import table_index_meta;
import table_entry;

namespace infinity {

void PhysicalOptimize::Init() {}

bool PhysicalOptimize::Execute(QueryContext *query_context, OperatorState *operator_state) {
    if (index_name_.empty()) {
        OptimizeIndex(query_context, operator_state);
    } else {
        OptimizeAIndex(query_context, operator_state);
    }

    operator_state->SetComplete();
    return true;
}

void PhysicalOptimize::OptimizeIndex(QueryContext *query_context, OperatorState *operator_state) {
    // Get tables from catalog
    LOG_INFO(fmt::format("OptimizeIndex {}.{} begin", db_name_, table_name_));
    auto txn = query_context->GetTxn();
    auto [table_entry, table_status] = txn->GetTableByName(db_name_, table_name_);

    if (!table_status.ok()) {
        operator_state->status_ = table_status;
        RecoverableError(table_status);
        return;
    }

    table_entry->OptimizeIndex(txn);
    LOG_INFO(fmt::format("OptimizeIndex {}.{} end", db_name_, table_name_));
}

void PhysicalOptimize::OptimizeAIndex(QueryContext *query_context, OperatorState *operator_state) {
    auto txn = query_context->GetTxn();
    auto [table_index_entry, status] = txn->GetIndexByName(db_name_, table_name_, index_name_);
    if (!status.ok()) {
        operator_state->status_ = status;
        RecoverableError(status);
        return;
    }
    table_index_entry->OptimizeIndex(txn, opt_params_);
}

} // namespace infinity
