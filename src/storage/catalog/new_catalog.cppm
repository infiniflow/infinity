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

module;

export module new_catalog;


import stl;
import table_def;
//import function;
//import function_set;
//import table_function;
//import special_function;
//import third_party;
//import buffer_manager;
//import profiler;
import status;
//import default_values;
import meta_info;
//import index_base;
//import txn_store;
//import data_access_state;
import extra_ddl_info;
//import db_entry;
//import table_entry;
//import table_index_entry;
//import segment_entry;
//import db_meta;
//import meta_map;
//import base_entry;
//import column_def;
//import cleanup_scanner;
//import log_file;
//import snapshot_info;


namespace infinity {

export struct NewCatalog {
public:
    NewCatalog();
    ~NewCatalog();

public:
    // Database related functions
    Status CreateDatabase(const SharedPtr<String> &db_name,
                          const SharedPtr<String> &comment,
                          TransactionID txn_id,
                          TxnTimeStamp begin_ts,
                          ConflictType conflict_type = ConflictType::kError);

    Status DropDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts, ConflictType conflict_type = ConflictType::kError);

    bool CheckDatabaseExists(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    //    void RemoveDBEntry(DBEntry *db_entry, TransactionID txn_id);
    //
    //    // replay
    //    void
    //    CreateDatabaseReplay(const SharedPtr<String> &db_name,
    //                         const SharedPtr<String> &comment,
    //                         std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)>
    //                         &&init_entry, TransactionID txn_id, TxnTimeStamp begin_ts);
    //
    //    void DropDatabaseReplay(const String &db_name,
    //                            std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    //                            TransactionID txn_id,
    //                            TxnTimeStamp begin_ts);
    //
    //    DBEntry *GetDatabaseReplay(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

private:

};

} // namespace infinity