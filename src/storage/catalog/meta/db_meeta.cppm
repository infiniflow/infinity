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

export module infinity_core:db_meeta;

import :stl;
import :status;
import :new_catalog;

namespace infinity {

class KVInstance;
struct DatabaseInfo;
class NewTxn;
class MetaCache;
struct MetaTableCache;

export class DBMeeta {
public:
    DBMeeta(String db_id_str, NewTxn *txn);
    DBMeeta(String db_id_str, KVInstance *kv_instance, MetaCache *meta_cache);

    const String &db_id_str() const;

    KVInstance *kv_instance() const { return kv_instance_; }

    Status InitSet(const String *comment = nullptr);

    Status UninitSet(UsageFlag usage_flag);

    Status GetTableIDs(Vector<String> *&table_id_strs, Vector<String> **table_names = nullptr);

    Status GetTableID(const String &table_name, String &table_key, String &table_id_str, TxnTimeStamp &create_table_ts, SharedPtr<MetaTableCache>& table_cache);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo();

    Tuple<String, Status> GetNextTableID();

    Status SetNextTableID(const String &table_id_str);

    String GetDBName() const { return db_name_; }
    void SetDBName(const String &db_name) { db_name_ = db_name; }

private:
    Status GetComment(String *&comment);
    Status LoadTableIDs();
    String GetDBTag(const String &tag) const;

private:
    mutable std::mutex mtx_;

    String db_id_str_{};
    String db_name_{};
    NewTxn *txn_{};
    TxnTimeStamp txn_begin_ts_{};
    KVInstance *kv_instance_{};
    MetaCache *meta_cache_{};

    Optional<String> comment_;
    Optional<Vector<String>> table_id_strs_;
    Optional<Vector<String>> table_names_;
};

} // namespace infinity
