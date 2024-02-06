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

export module db_entry;

import stl;
import table_entry_type;
import table_meta;
import base_entry;
import table_entry;
import third_party;
import table_detail;
import buffer_manager;
import status;
import extra_ddl_info;
import column_def;

namespace infinity {

class TxnManager;

export class DBEntry : public BaseEntry {
    friend struct NewCatalog;

public:
    explicit DBEntry(const SharedPtr<String> &data_dir, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<DBEntry>
    NewDBEntry(const SharedPtr<String> &data_dir, const SharedPtr<String> &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<DBEntry> NewReplayDBEntry(const SharedPtr<String> &data_dir,
                                               const SharedPtr<String> &db_name,
                                               TransactionID txn_id,
                                               TxnTimeStamp begin_ts,
                                               TxnTimeStamp commit_ts,
                                               bool is_delete);

public:
    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<DBEntry> Deserialize(const nlohmann::json &db_entry_json, BufferManager *buffer_mgr);

    virtual void MergeFrom(BaseEntry &other);

    [[nodiscard]] const String &db_name() const { return *db_name_; }

    [[nodiscard]] const SharedPtr<String> &db_name_ptr() const { return db_name_; }

    [[nodiscard]] const String &db_entry_dir() const { return *db_entry_dir_; }

    [[nodiscard]] const SharedPtr<String> &db_entry_dir_ptr() const { return db_entry_dir_; }

    [[nodiscard]] HashMap<String, UniquePtr<TableMeta>> &tables() { return tables_; }

private:
    Tuple<TableEntry *, Status> CreateTable(TableEntryType table_entry_type,
                                            const SharedPtr<String> &table_collection_name,
                                            const Vector<SharedPtr<ColumnDef>> &columns,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnManager *txn_mgr);

    Tuple<TableEntry *, Status>
    DropTable(const String &table_collection_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableEntry *, Status> GetTableCollection(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    void RemoveTableEntry(const String &table_collection_name, TransactionID txn_id, TxnManager *txn_mgr);

    Vector<TableEntry *> TableCollections(TransactionID txn_id, TxnTimeStamp begin_ts);

    Status GetTablesDetail(TransactionID txn_id, TxnTimeStamp begin_ts, Vector<TableDetail> &output_table_array);

private:
    std::shared_mutex rw_locker_{};
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> db_name_{};
    HashMap<String, UniquePtr<TableMeta>> tables_{};
};
} // namespace infinity
