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

import stl;
import txn;
import query_context;
import parser;
import operator_state;
import infinity_exception;
import base_entry;
import table_collection_entry;
import table_def;
import third_party;
import status;
import logger;
import iresearch_datastore;
import irs_index_entry;
import base_table_ref;
import table_index_entry;
import table_index_meta;

module physical_optimize;

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
    u64 txn_id = txn->TxnID();
    LOG_INFO(Format("OptimizeIndex {} {}", db_name_, object_name_));
    TxnTimeStamp begin_ts = query_context->GetTxn()->BeginTS();
    auto [base_table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        operator_state->error_message_ = Move(status.msg_);
        Error<ExecutorException>(Format("{} isn't found", object_name_));
        return;
    }

    auto table_collection_entry = dynamic_cast<TableCollectionEntry *>(base_table_entry);
    SharedPtr<IrsIndexEntry> irs_index_entry;
    for (auto &[index_name, table_index_meta] : table_collection_entry->index_meta_map_) {
        BaseEntry *base_entry{nullptr};
        auto status = TableIndexMeta::GetEntry(table_index_meta.get(), txn_id, begin_ts, base_entry);
        if (!status.ok()) {
            Error<StorageException>("Cannot find index entry.");
        }
        TableIndexEntry *table_index_entry = (TableIndexEntry *)base_entry;
        irs_index_entry = table_index_entry->irs_index_entry_;
    }
    if (irs_index_entry.get()) {
        LOG_INFO(Format("ScheduleOptimize"));
        irs_index_entry->irs_index_->ScheduleOptimize();
    }
    LOG_TRACE("Optimize index");
}

} // namespace infinity
