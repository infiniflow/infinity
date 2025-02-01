// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

// #include <cassert>
// #include <filesystem>
// #include <fstream>
// #include <thread>
// #include <vector>

module new_catalog;
//
import stl;
// import defer_op;
// import data_type;
// import txn_manager;
// import logger;
// import third_party;
import status;
// import infinity_exception;
// import function_set;
// import scalar_function_set;
// import table_function;
// import special_function;
// import buffer_manager;
// import column_def;
// import virtual_store;
// import table_def;
// import table_entry_type;
// import meta_info;
// import index_base;
// import txn_store;
// import data_access_state;
// import catalog_delta_entry;
// import file_writer;
import extra_ddl_info;
// import index_defines;
// import infinity_context;
// import create_index_info;
// import persistence_manager;
//
// import table_meta;
// import table_index_meta;
// import base_entry;
// import block_entry;
// import block_column_entry;
// import segment_index_entry;
// import chunk_index_entry;
// import log_file;
// import persist_result_handler;
// import local_file_handle;
// import admin_statement;
// import global_resource_usage;
// import snapshot_info;

namespace infinity {

Status NewCatalog::CreateDatabase(const SharedPtr<String> &db_name,
                                  const SharedPtr<String> &comment,
                                  TransactionID txn_id,
                                  TxnTimeStamp begin_ts,
                                  ConflictType conflict_type) {
    return Status::OK();
}

Status NewCatalog::DropDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, ConflictType conflict_type) {
    return Status::OK();
}

bool NewCatalog::CheckDatabaseExists(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) { return true; }

Tuple<SharedPtr<DatabaseInfo>, Status> NewCatalog::GetDatabaseInfo(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    return {nullptr, Status::OK()};
}

} // namespace infinity