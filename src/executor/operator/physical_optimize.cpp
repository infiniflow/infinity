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
import iresearch_datastore;
import base_table_ref;
import irs_index_entry;

namespace infinity {

void PhysicalOptimize::Init() {}

bool PhysicalOptimize::Execute(QueryContext *query_context, OperatorState *operator_state) {
    switch (optimize_type_) {
        case OptimizeType::kIRS: {
            OptimizeIndex(query_context, operator_state);
            break;
        }
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalOptimize::OptimizeIndex(QueryContext *query_context, OperatorState *operator_state) {
    // Get tables from catalog
    auto txn = query_context->GetTxn();
    TransactionID txn_id = txn->TxnID();
    LOG_INFO(fmt::format("OptimizeIndex {} {}", db_name_, object_name_));
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    auto [table_entry, table_status] = txn->GetTableByName(db_name_, object_name_);

    if(!table_status.ok()) {
        operator_state->status_ = table_status;
        RecoverableError(table_status);
        return ;
    }

    SharedPtr<IrsIndexEntry> irs_index_entry;
    for (auto &[index_name, table_index_meta] : table_entry->index_meta_map()) {
        auto [table_index_entry, index_status] = table_index_meta->GetEntry(txn_id, begin_ts);
        if (!index_status.ok()) {
            operator_state->status_ = index_status;
            RecoverableError(index_status);
        }
        irs_index_entry = table_index_entry->irs_index_entry();
    }
    if (irs_index_entry) {
        LOG_INFO(fmt::format("ScheduleOptimize"));
        irs_index_entry->irs_index_->ScheduleOptimize();
    }
    LOG_TRACE("Optimize index");
}

} // namespace infinity
