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
import meta_info;
import buffer_manager;
import status;
import extra_ddl_info;
import column_def;
import meta_map;
import random;
import meta_entry_interface;
import cleanup_scanner;

namespace infinity {

class TxnManager;
class Txn;
class DBMeta;

export class DBEntry final : public BaseEntry, public EntryInterface {
    friend struct Catalog;

    static String EncodeIndex(const String &db_name);

public:
    explicit DBEntry(DBMeta *db_meta,
                     bool is_delete,
                     const SharedPtr<String> &db_entry_dir,
                     const SharedPtr<String> &db_name,
                     TransactionID txn_id,
                     TxnTimeStamp begin_ts);

    static SharedPtr<DBEntry> NewDBEntry(DBMeta *db_meta,
                                         bool is_delete,
                                         const SharedPtr<String> &data_dir,
                                         const SharedPtr<String> &db_name,
                                         TransactionID txn_id,
                                         TxnTimeStamp begin_ts);

    static SharedPtr<DBEntry> ReplayDBEntry(DBMeta *db_meta,
                                            bool is_delete,
                                            const SharedPtr<String> &db_entry_dir,
                                            const SharedPtr<String> &db_name,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnTimeStamp commit_ts) noexcept;

public:
    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<DBEntry> Deserialize(const nlohmann::json &db_entry_json, DBMeta *db_meta, BufferManager *buffer_mgr);

    [[nodiscard]] const SharedPtr<String> &db_name_ptr() const { return db_name_; }

    [[nodiscard]] const SharedPtr<String> &db_entry_dir() const { return db_entry_dir_; }

    String GetPathNameTail() const;

    Tuple<TableEntry *, Status> CreateTable(TableEntryType table_entry_type,
                                            const SharedPtr<String> &table_collection_name,
                                            const Vector<SharedPtr<ColumnDef>> &columns,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnManager *txn_mgr,
                                            ConflictType conflict_type);

    Tuple<SharedPtr<TableEntry>, Status>
    DropTable(const String &table_collection_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableEntry *, Status> GetTableCollection(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    bool CheckConflict(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts, TableEntry *&table_entry);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    void RemoveTableEntry(const String &table_collection_name, TransactionID txn_id);

    // replay
    void CreateTableReplay(const SharedPtr<String> &table_name,
                           std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                           TransactionID txn_id,
                           TxnTimeStamp begin_ts);

    void UpdateTableReplay(const SharedPtr<String> &table_name,
                           std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                           TransactionID txn_id,
                           TxnTimeStamp begin_ts);

    void DropTableReplay(const String &table_name,
                         std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                         TransactionID txn_id,
                         TxnTimeStamp begin_ts);

    TableEntry *GetTableReplay(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts);
    //

    Vector<TableEntry *> TableCollections(TransactionID txn_id, TxnTimeStamp begin_ts);

    Status GetTablesDetail(TransactionID txn_id, TxnTimeStamp begin_ts, Vector<TableDetail> &output_table_array);

private:
    static SharedPtr<String> DetermineDBDir(const String &parent_dir, const String &db_name) {
        return DetermineRandomString(parent_dir, fmt::format("db_{}", db_name));
    }

    friend void RecreateSegmentSealingTasksInStorageInit(Catalog *catalog, TxnManager *txn_mgr, TxnTimeStamp system_start_ts);

public:
    DBMeta *const db_meta_;

private:
    const SharedPtr<String> db_entry_dir_{};
    const SharedPtr<String> db_name_{};

    MetaMap<TableMeta> table_meta_map_{};

private: // TODO: remote it
    std::shared_mutex &rw_locker() { return table_meta_map_.rw_locker_; }

    HashMap<String, UniquePtr<TableMeta>> &table_meta_map() { return table_meta_map_.meta_map_; }

public:
    void PickCleanup(CleanupScanner *scanner) override;

    void Cleanup() override;

    void MemIndexCommit();
    void MemIndexRecover(BufferManager *buffer_manager);
};
} // namespace infinity
