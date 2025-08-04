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
import :base_meta;

namespace infinity {

class KVInstance;
struct DatabaseInfo;
class NewTxn;

export class DBMeeta : public BaseMeta {
public:
    explicit DBMeeta(String db_id_str);

    const String &db_id_str() const;

    Status InitSet(KVInstance *kv_instance, const String *comment = nullptr);

    Status UninitSet(KVInstance *kv_instance, UsageFlag usage_flag);

    Status GetTableIDs(KVInstance *kv_instance, TxnTimeStamp begin_ts, Vector<String> *&table_id_strs, Vector<String> **table_names = nullptr);

    Status GetTableID(KVInstance *kv_instance,
                      TxnTimeStamp begin_ts,
                      const String &table_name,
                      String &table_key,
                      String &table_id_str,
                      TxnTimeStamp &create_table_ts);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(KVInstance *kv_instance);

    Tuple<String, Status> GetNextTableID(KVInstance *kv_instance);

private:
    Status GetComment(KVInstance *kv_instance, String *&comment);
    Status LoadTableIDs(KVInstance *kv_instance, TxnTimeStamp begin_ts);
    String GetDBTag(const String &tag) const;

private:
    mutable std::mutex mtx_;

    String db_id_str_{};
    String db_name_{};

    Optional<String> comment_;
    Optional<Vector<String>> table_id_strs_;
    Optional<Vector<String>> table_names_;
};

} // namespace infinity
