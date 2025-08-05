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

module infinity_core:database_snapshot.impl;

import :snapshot;

import :stl;
import :new_txn;
import :new_txn_manager;
import :query_context;
import :status;
import :third_party;
import :config;
import :infinity_exception;
import :snapshot_info;
import :db_meeta;
import :table_meeta;
import :txn_state;
import :logger;

namespace infinity {

Status Snapshot::CreateDatabaseSnapshot(QueryContext *query_context, const String &snapshot_name, const String &db_name) {
    auto *txn_ptr = query_context->GetNewTxn();
    auto *txn_mgr = txn_ptr->txn_mgr();

    SharedPtr<DatabaseSnapshotInfo> database_snapshot;
    Status status;
    std::tie(database_snapshot, status) = txn_ptr->GetDatabaseSnapshotInfo(db_name);
    if (!status.ok()) {
        RecoverableError(status);
    }
    database_snapshot->snapshot_name_ = snapshot_name;
    String snapshot_dir = query_context->global_config()->SnapshotDir();
    status = database_snapshot->Serialize(snapshot_dir, txn_mgr->GetReadCommitTS(txn_ptr));
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status Snapshot::RestoreDatabaseSnapshot(QueryContext *query_context, const String &snapshot_name) {
    auto *txn_ptr = query_context->GetNewTxn();
    String snapshot_dir = query_context->global_config()->SnapshotDir();

    SharedPtr<DatabaseSnapshotInfo> database_snapshot;
    Status status;
    std::tie(database_snapshot, status) = DatabaseSnapshotInfo::Deserialize(snapshot_dir, snapshot_name);
    if (!status.ok()) {
        return status;
    }

    // check txn_type
    LOG_INFO(fmt::format("txn type: {}", TransactionType2Str(txn_ptr->GetTxnType())));
    // if (txn_ptr->GetTxnType() != TransactionType::kRestoreTable) {
    //     return Status::InvalidArgument("Txn type is not RestoreTable");
    // }

    status = txn_ptr->RestoreDatabaseSnapshot(database_snapshot);
    if (!status.ok()) {
        return status;
    }

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