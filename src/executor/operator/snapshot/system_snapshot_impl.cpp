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

module infinity_core:system_snapshot.impl;

import :snapshot;
import :new_txn;
import :new_txn_manager;
import :query_context;
import :status;
import :config;
import :infinity_exception;
import :snapshot_info;
import :db_meta;
import :table_meta;
import :txn_state;
import :logger;
import :kv_store;
import :kv_code;
import :infinity_context;

import third_party;

namespace infinity {
Status Snapshot::RestoreSystemSnapshot(QueryContext *query_context, const std::string &snapshot_name) {
    auto *storage = InfinityContext::instance().storage();
    auto *kv_store = storage->kv_store();
    std::string drop_key = KeyEncode::DropSnapshotKey(snapshot_name);
    std::string drop_ts_str;
    Status status = kv_store->Get(drop_key, drop_ts_str);
    if (status.ok()) {
        return Status::NotFound(fmt::format("Snapshot: {} has been dropped", snapshot_name));
    }

    auto *txn_ptr = query_context->GetNewTxn();
    std::string snapshot_dir = query_context->global_config()->SnapshotDir();

    std::shared_ptr<SystemSnapshotInfo> system_snapshot;
    std::tie(system_snapshot, status) = SystemSnapshotInfo::Deserialize(snapshot_dir, snapshot_name);
    if (!status.ok()) {
        return status;
    }

    LOG_INFO(fmt::format("txn type: {}", TransactionType2Str(txn_ptr->GetTxnType())));

    status = txn_ptr->RestoreSystemSnapshot(system_snapshot);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

} // namespace infinity