// // Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     https://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.
//
// module;
//
// module snapshot;
//
// import stl;
// import txn;
// import query_context;
// import table_entry;
// import status;
// import third_party;
// import config;
// import infinity_exception;
// import snapshot_info;
//
// namespace infinity {
//
// Status Snapshot::CreateDatabaseSnapshot(QueryContext *query_context, const String &snapshot_name) {
//     Txn *txn_ptr = query_context->GetTxn();
//     const String &db_name = query_context->schema_name();
//
//
//     SharedPtr<DatabaseSnapshotInfo> db_snapshot;
//     db_snapshot->snapshot_name_ = snapshot_name;
//     db_snapshot->db_name_ = db_name;
//
//
//     Vector<String> table_names = txn_ptr->GetTableNames();
//     for (const auto &table_name : table_names) {
//         SharedPtr<TableSnapshotInfo> table_snapshot;
//         Status status;
//         std::tie(table_snapshot, status) = txn_ptr->GetTableSnapshot(db_name, table_name);
//         if (!status.ok()) {
//             LOG_WARN(fmt::format("Failed to get table snapshot {}: {}", table_name, status.message()));
//             continue;
//         }
//         table_snapshot->snapshot_name_ = snapshot_name;
//         db_snapshot->table_snapshots_[table_name] = table_snapshot;
//     }
//
//
//     String snapshot_dir = query_context->global_config()->SnapshotDir();
//     db_snapshot->Serialize(snapshot_dir);
//
//     return Status::OK();
// }
//
// void RestoreDatabaseSnapshot(QueryContext *query_context, const String &snapshot_name) {
//     auto snapshot_dir = query_context->global_config()->SnapshotDir();
//
//
//     auto [db_snapshot, status] = DatabaseSnapshotInfo::Deserialize(snapshot_dir, snapshot_name);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Failed to deserialize database snapshot: {}", status.message()));
//         return;
//     }
//
//
//     for (const auto &[table_name, table_snapshot] : db_snapshot->table_snapshots_) {
//         auto txn = query_context->GetTxn();
//         txn->ApplyTableSnapshot(table_snapshot);
//     }
//
//     LOG_INFO(fmt::format("Database snapshot restored successfully: {}", snapshot_name));
// }
//
// } // namespace infinity