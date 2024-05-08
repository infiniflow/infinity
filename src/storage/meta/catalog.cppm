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

export module catalog;

import stl;
import table_def;
import function;
import function_set;
import table_function;
import special_function;
import third_party;
import buffer_manager;
import profiler;
import status;
import default_values;
import meta_info;
import index_base;
import txn_store;
import data_access_state;
import extra_ddl_info;
import db_entry;
import table_entry;
import table_index_entry;
import segment_entry;
import db_meta;
import meta_map;
import base_entry;

import meta_entry_interface;
import cleanup_scanner;
import log_file;

namespace infinity {

class TxnManager;
class Txn;
class ProfileHistory {
private:
    std::mutex lock_{};
    Vector<SharedPtr<QueryProfiler>> queue{};
    SizeT front{};
    SizeT rear{};
    SizeT max_size{};

public:
    explicit ProfileHistory(SizeT size) {
        max_size = size + 1;
        queue.resize(max_size);
        front = 0;
        rear = 0;
    }

    void Enqueue(SharedPtr<QueryProfiler> &&profiler) {
        std::unique_lock<std::mutex> lk(lock_);
        if ((rear + 1) % max_size == front) {
            return;
        }
        queue[rear] = profiler;
        rear = (rear + 1) % max_size;
    }

    QueryProfiler *GetElement(SizeT index) {
        std::unique_lock<std::mutex> lk(lock_);

        // FIXME: Bug, unsigned integer: index will always >= 0
        if (index < 0 || index >= (rear - front + max_size) % max_size) {
            return nullptr;
        }
        SizeT actualIndex = (front + index) % max_size;
        return queue[actualIndex].get();
    }

    Vector<SharedPtr<QueryProfiler>> GetElements() {
        Vector<SharedPtr<QueryProfiler>> elements;
        elements.reserve(max_size);

        std::unique_lock<std::mutex> lk(lock_);
        for (SizeT i = 0; i < queue.size(); ++i) {
            if (queue[i].get() != nullptr) {
                elements.push_back(queue[i]);
            }
        }
        return elements;
    }
};

class GlobalCatalogDeltaEntry;
class CatalogDeltaEntry;
export struct Catalog {
public:
    explicit Catalog(SharedPtr<String> data_dir);

    ~Catalog();

public:
    // Database related functions
    Tuple<DBEntry *, Status> CreateDatabase(const String &db_name,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnManager *txn_mgr,
                                            ConflictType conflict_type = ConflictType::kError);

    Tuple<SharedPtr<DBEntry>, Status> DropDatabase(const String &db_name,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnManager *txn_mgr,
                                                   ConflictType conflict_type = ConflictType::kError);

    Tuple<DBEntry *, Status> GetDatabase(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    void RemoveDBEntry(DBEntry *db_entry, TransactionID txn_id);

    // replay
    void CreateDatabaseReplay(const SharedPtr<String> &db_name,
                              std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                              TransactionID txn_id,
                              TxnTimeStamp begin_ts);

    void DropDatabaseReplay(const String &db_name,
                            std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                            TransactionID txn_id,
                            TxnTimeStamp begin_ts);

    DBEntry *GetDatabaseReplay(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    // List databases
    Vector<DBEntry *> Databases(TransactionID txn_id, TxnTimeStamp begin_ts);

    // Table related functions
    Tuple<TableEntry *, Status> CreateTable(const String &db_name,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            const SharedPtr<TableDef> &table_def,
                                            ConflictType conflict_type,
                                            TxnManager *txn_mgr);

    Tuple<SharedPtr<TableEntry>, Status> DropTableByName(const String &db_name,
                                                         const String &table_name,
                                                         ConflictType conflict_type,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         TxnManager *txn_mgr);

    Status GetTables(const String &db_name, Vector<TableDetail> &output_table_array, Txn *txn);

    Tuple<TableEntry *, Status> GetTableByName(const String &db_name, const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &db_name, const String &table_name, Txn *txn);

    static Status RemoveTableEntry(TableEntry *table_entry, TransactionID txn_id);

    // Index Related methods
    Tuple<TableIndexEntry *, Status> CreateIndex(TableEntry *table_entry,
                                                 const SharedPtr<IndexBase> &index_base,
                                                 ConflictType conflict_type,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr);

    Tuple<SharedPtr<TableIndexEntry>, Status> DropIndex(const String &db_name,
                                                        const String &table_name,
                                                        const String &index_name,
                                                        ConflictType conflict_type,
                                                        TransactionID txn_id,
                                                        TxnTimeStamp begin_ts,
                                                        TxnManager *txn_mgr);

    Tuple<TableIndexEntry *, Status>
    GetIndexByName(const String &db_name, const String &table_name, const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<SharedPtr<TableIndexInfo>, Status>
    GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    static Status RemoveIndexEntry(TableIndexEntry *table_index_entry, TransactionID txn_id);

    static void CommitCreateIndex(TxnIndexStore *txn_index_store, TxnTimeStamp commit_ts, bool is_replay = false);

    // static void RollbackPopulateIndex(TxnIndexStore *txn_index_store, Txn *txn);

    // Append related functions
    static void
    Append(TableEntry *table_entry, TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, BufferManager *buffer_mgr, bool is_replay = false);

    static void RollbackAppend(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store);

    static Status Delete(TableEntry *table_entry, TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, DeleteState &delete_state);

    static Status RollbackDelete(TableEntry *table_entry, TransactionID txn_id, DeleteState &append_state, BufferManager *buffer_mgr);

    static Status CommitCompact(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_store);

    static Status RollbackCompact(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store);

    static Status
    CommitWrite(TableEntry *table_entry, TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<SegmentID, TxnSegmentStore> &segment_stores);

    static Status RollbackWrite(TableEntry *table_entry, TxnTimeStamp commit_ts, const Vector<TxnSegmentStore> &segment_stores);

    static SegmentID GetNextSegmentID(TableEntry *table_entry);

    // This not add row count
    static void AddSegment(TableEntry *table_entry, SharedPtr<SegmentEntry> &segment_entry);

public:
    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(Catalog *catalog, String function_name);

    static void AddFunctionSet(Catalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void AddSpecialFunction(Catalog *catalog, const SharedPtr<SpecialFunction> &special_function);

    static Tuple<SpecialFunction *, Status> GetSpecialFunctionByNameNoExcept(Catalog *catalog, String function_name);

public:
    // Serialization and Deserialization
    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    void SaveFullCatalog(TxnTimeStamp max_commit_ts, String &full_path);

    bool SaveDeltaCatalog(TxnTimeStamp max_commit_ts, String &delta_path);

    void AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry, i64 wal_size);

    void ReplayDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    static UniquePtr<Catalog> NewCatalog(SharedPtr<String> data_dir, bool create_default_db);

    static UniquePtr<Catalog>
    LoadFromFiles(const FullCatalogFileInfo &full_ckp_info, const Vector<DeltaCatalogFileInfo> &delta_ckp_infos, BufferManager *buffer_mgr);

    SizeT GetDeltaLogCount() const;

private:
    static UniquePtr<Catalog> Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr);

    static UniquePtr<CatalogDeltaEntry> LoadFromFileDelta(const DeltaCatalogFileInfo &delta_ckp_info);

    void LoadFromEntryDelta(TxnTimeStamp max_commit_ts, BufferManager *buffer_mgr);

    static UniquePtr<Catalog> LoadFromFile(const FullCatalogFileInfo &full_ckp_info, BufferManager *buffer_mgr);

public:
    // Profile related methods

    void AppendProfilerRecord(SharedPtr<QueryProfiler> profiler) { history.Enqueue(std::move(profiler)); }

    const QueryProfiler *GetProfilerRecord(SizeT index) { return history.GetElement(index); }

    const Vector<SharedPtr<QueryProfiler>> GetProfilerRecords() { return history.GetElements(); }

public:
    const SharedPtr<String> &DataDir() const { return data_dir_; }

    const SharedPtr<String> &CatalogDir() const { return catalog_dir_; }

    TransactionID next_txn_id() const { return next_txn_id_; }

public:
    SharedPtr<String> data_dir_{};
    SharedPtr<String> catalog_dir_{};

    MetaMap<DBMeta> db_meta_map_{};

    Atomic<TransactionID> next_txn_id_{};

private:
    TxnTimeStamp full_ckp_commit_ts_{};

public:
    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_{};
    HashMap<String, SharedPtr<SpecialFunction>> special_functions_{};

    ProfileHistory history{DEFAULT_PROFILER_HISTORY_SIZE};

private: // TODO: remove this
    std::shared_mutex &rw_locker() { return db_meta_map_.rw_locker_; }

    HashMap<String, UniquePtr<DBMeta>> &db_meta_map() { return db_meta_map_.meta_map_; };

    Atomic<bool> running_{};
    Thread mem_index_commit_thread_{};

    void MemIndexCommit();

    void MemIndexCommitLoop();

public:
    void MemIndexRecover(BufferManager *buffer_manager);

    void PickCleanup(CleanupScanner *scanner);

    // delta checkpoint info
public:
    Tuple<TxnTimeStamp, i64> GetCheckpointState() const;

    void InitDeltaEntry(TxnTimeStamp max_commit_ts);

private:
    UniquePtr<GlobalCatalogDeltaEntry> global_catalog_delta_entry_{MakeUnique<GlobalCatalogDeltaEntry>()};
};

} // namespace infinity
