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
import cleanup_scanner;

namespace infinity {

class TxnManager;
class Txn;
class DBMeta;

export struct DBEntry final : public BaseEntry {
    friend struct Catalog;

public:
    static Vector<std::string_view> DecodeIndex(std::string_view encode);

    static String EncodeIndex(const String &db_name);

public:
    explicit DBEntry(DBMeta *db_meta,
                     bool is_delete,
                     const SharedPtr<String> &db_entry_dir,
                     const SharedPtr<String> &db_name,
                     const SharedPtr<String> &comment,
                     TransactionID txn_id,
                     TxnTimeStamp begin_ts);

    static SharedPtr<DBEntry> NewDBEntry(DBMeta *db_meta,
                                         bool is_delete,
                                         const SharedPtr<String> &db_name,
                                         const SharedPtr<String> &comment,
                                         TransactionID txn_id,
                                         TxnTimeStamp begin_ts);

    static SharedPtr<DBEntry> ReplayDBEntry(DBMeta *db_meta,
                                            bool is_delete,
                                            const SharedPtr<String> &db_entry_dir,
                                            const SharedPtr<String> &db_name,
                                            const SharedPtr<String> &comment,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnTimeStamp commit_ts) noexcept;

public:
    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<DBEntry> Deserialize(const nlohmann::json &db_entry_json, DBMeta *db_meta, BufferManager *buffer_mgr);

    [[nodiscard]] const SharedPtr<String> &db_name_ptr() const { return db_name_; }

    [[nodiscard]] const SharedPtr<String> &db_entry_dir() const { return db_entry_dir_; }

    [[nodiscard]] const SharedPtr<String> &db_comment_ptr() const { return db_comment_; }

    SharedPtr<String> AbsoluteDir() const;

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

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &table_name, Txn *txn);

    void RemoveTableEntry(const String &table_collection_name, TransactionID txn_id);

    Status AddTable(SharedPtr<TableEntry> table_entry, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, bool add_if_found = false);

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

    Vector<TableEntry *> TableCollections(TransactionID txn_id, TxnTimeStamp begin_ts);

    Status GetTablesDetail(Txn *txn, Vector<TableDetail> &output_table_array);

    Tuple<Vector<String>, Vector<TableMeta *>, std::shared_lock<std::shared_mutex>> GetAllTableMetas() const;

private:
    static SharedPtr<String> DetermineDBDir(const String &db_name);

    friend void RecreateSegmentSealingTasksInStorageInit(Catalog *catalog, TxnManager *txn_mgr, TxnTimeStamp system_start_ts);

public:
    DBMeta *const db_meta_;

private:
    const SharedPtr<String> db_entry_dir_{};
    const SharedPtr<String> db_name_{};
    const SharedPtr<String> db_comment_{};

    MetaMap<TableMeta> table_meta_map_{};

private: // TODO: remote it
    std::shared_mutex &GetTableMetaLock() { return table_meta_map_.GetMetaLock(); }

    //    HashMap<String, UniquePtr<TableMeta>> &table_meta_map() { return table_meta_map_.meta_map_; }

public:
    void PickCleanup(CleanupScanner *scanner) override;

    Vector<String> GetFilePath(TransactionID txn_id, TxnTimeStamp begin_ts) const final;

    void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true) override;

    void MemIndexCommit();
    void MemIndexRecover(BufferManager *buffer_manager, TxnTimeStamp ts);
};
} // namespace infinity
