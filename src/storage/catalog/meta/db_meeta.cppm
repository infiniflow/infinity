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

export module infinity_core:db_meeta;

import :status;
import :new_catalog;

namespace infinity {

class KVInstance;
struct DatabaseInfo;
class NewTxn;

export class DBMeeta {
public:
    DBMeeta(std::string db_id_str, NewTxn *txn);
    DBMeeta(std::string db_id_str, KVInstance *kv_instance);

    const std::string &db_id_str() const;

    KVInstance *kv_instance() const { return kv_instance_; }

    Status InitSet(const std::string *comment = nullptr);

    Status UninitSet(UsageFlag usage_flag);

    Status GetTableIDs(std::vector<std::string> *&table_id_strs, std::vector<std::string> **table_names = nullptr);

    Status GetTableID(const std::string &table_name, std::string &table_key, std::string &table_id_str, TxnTimeStamp &create_table_ts);

    std::tuple<std::shared_ptr<DatabaseInfo>, Status> GetDatabaseInfo();

    std::tuple<std::string, Status> GetNextTableID();

    Status SetNextTableID(const std::string &table_id_str);

    std::string GetDBName() const { return db_name_; }
    void SetDBName(const std::string &db_name) { db_name_ = db_name; }

private:
    Status GetComment(std::string *&comment);
    Status LoadTableIDs();
    std::string GetDBTag(const std::string &tag) const;

private:
    mutable std::mutex mtx_;

    std::string db_id_str_{};
    std::string db_name_{};
    NewTxn *txn_{};
    TxnTimeStamp txn_begin_ts_{};
    KVInstance *kv_instance_{};

    std::optional<std::string> comment_;
    std::optional<std::vector<std::string>> table_id_strs_;
    std::optional<std::vector<std::string>> table_names_;
};

} // namespace infinity
