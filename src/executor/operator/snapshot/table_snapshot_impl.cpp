// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:table_snapshot.impl;

import :snapshot;
import :new_txn;
import :new_txn_manager;
import :query_context;
import :status;
import :config;
import :infinity_exception;
import :snapshot_info;
import :db_meeta;
import :table_meeta;
import :txn_state;
import :logger;

import third_party;

namespace infinity {

Status Snapshot::CreateTableSnapshot(QueryContext *query_context, const std::string &snapshot_name, const std::string &table_name) {
    // auto *txn_ptr = query_context->GetNewTxn();
    // const std::string &db_name = query_context->schema_name();

    // // Start timing for overall snapshot creation
    // auto snapshot_creation_start = std::chrono::high_resolution_clock::now();

    // std::shared_ptr<TableSnapshotInfo> table_snapshot;
    // Status status;
    // std::tie(table_snapshot, status) = txn_ptr->GetTableSnapshotInfo(db_name, table_name);
    // if (!status.ok()) {
    //     RecoverableError(status);
    // }
    // table_snapshot->snapshot_name_ = snapshot_name;
    // std::string snapshot_dir = query_context->global_config()->SnapshotDir();
    // status = table_snapshot->Serialize(snapshot_dir, txn_ptr->TxnID());
    // if (!status.ok()) {
    //     return status;
    // }

    // // End timing for overall snapshot creation
    // auto snapshot_creation_end = std::chrono::high_resolution_clock::now();
    // auto snapshot_creation_duration = std::chrono::duration_cast<std::chrono::milliseconds>(snapshot_creation_end - snapshot_creation_start);
    // LOG_INFO(fmt::format("Total snapshot creation took {} ms", snapshot_creation_duration.count()));

    return Status::OK();
}

Status Snapshot::RestoreTableSnapshot(QueryContext *query_context, const std::string &snapshot_name) {
    auto *txn_ptr = query_context->GetNewTxn();
    // might need to change this
    const std::string &db_name = query_context->schema_name();

    // Start timing for overall snapshot restoration
    // auto snapshot_restoration_start = std::chrono::high_resolution_clock::now();

    std::optional<DBMeeta> db_meta;
    TxnTimeStamp db_create_ts;
    Status status = txn_ptr->GetDBMeta(db_name, db_meta, db_create_ts);
    if (!status.ok()) {
        return status;
    }

    if (!db_meta.has_value()) {
        return Status::NotFound("DB not found");
    }
    std::string snapshot_dir = query_context->global_config()->SnapshotDir();

    std::shared_ptr<TableSnapshotInfo> table_snapshot;
    std::tie(table_snapshot, status) = TableSnapshotInfo::Deserialize(snapshot_dir, snapshot_name);
    if (!status.ok()) {
        return status;
    }

    // check txn_type
    LOG_INFO(fmt::format("txn type: {}", TransactionType2Str(txn_ptr->GetTxnType())));
    // if (txn_ptr->GetTxnType() != TransactionType::kRestoreTable) {
    //     return Status::InvalidArgument("Txn type is not RestoreTable");
    // }

    status = txn_ptr->RestoreTableSnapshot(db_name, table_snapshot);
    if (!status.ok()) {
        return status;
    }

    // End timing for overall snapshot restoration
    // auto snapshot_restoration_end = std::chrono::high_resolution_clock::now();
    // auto snapshot_restoration_duration = std::chrono::duration_cast<std::chrono::milliseconds>(snapshot_restoration_end - snapshot_restoration_start);
    // LOG_INFO(fmt::format("Total snapshot restoration took {} ms", snapshot_restoration_duration.count()));

    // print txn state
    // LOG_INFO(fmt::format("txn state: {}", TxnState2Str(txn_ptr->GetTxnState())));
    // txn_ptr->Commit();
    // LOG_INFO(fmt::format("txn state: {}", TxnState2Str(txn_ptr->GetTxnState())));
    //    if(!status.ok()) {
    //        return status;
    //    }
    //    txn_ptr->ApplyTableSnapshot(table_snapshot);
    return Status::OK();
}

} // namespace infinity