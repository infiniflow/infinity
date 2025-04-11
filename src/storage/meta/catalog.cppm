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
import extra_ddl_info;
import db_entry;
import table_entry;
import table_index_entry;
import segment_entry;
import db_meta;
import meta_map;
import base_entry;
import column_def;
import cleanup_scanner;
import log_file;
import snapshot_info;

namespace infinity {

class TxnManager;
class Txn;
struct CatalogDeltaOpBrief;
class SegmentIndexEntry;
struct DeleteState;

class GlobalCatalogDeltaEntry;
class CatalogDeltaEntry;
export struct Catalog {
public:
    Catalog();

    ~Catalog();

public:
    // Database related functions
    Tuple<DBEntry *, Status> CreateDatabase(const SharedPtr<String> &db_name,
                                            const SharedPtr<String> &comment,
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
    void
    CreateDatabaseReplay(const SharedPtr<String> &db_name,
                         const SharedPtr<String> &comment,
                         std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                         TransactionID txn_id,
                         TxnTimeStamp begin_ts);

    void DropDatabaseReplay(const String &db_name,
                            std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                            TransactionID txn_id,
                            TxnTimeStamp begin_ts);

    DBEntry *GetDatabaseReplay(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    // Start memory index commit thread
    void StartMemoryIndexCommit();

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

    Tuple<SharedPtr<TableSnapshotInfo>, Status> GetTableSnapshot(const String &db_name, const String &table_name, Txn *txn_ptr);

    Status ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info, Txn *txn_ptr);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &db_name, const String &table_name, Txn *txn);

    Tuple<SharedPtr<SegmentInfo>, Status> GetSegmentInfo(const String &db_name, const String &table_name, SegmentID segment_id, Txn *txn_ptr);

    Tuple<Vector<SharedPtr<SegmentInfo>>, Status> GetSegmentsInfo(const String &db_name, const String &table_name, Txn *txn_ptr);

    Tuple<SharedPtr<BlockInfo>, Status>
    GetBlockInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, Txn *txn);

    Tuple<Vector<SharedPtr<BlockInfo>>, Status> GetBlocksInfo(const String &db_name, const String &table_name, SegmentID segment_id, Txn *txn);

    Tuple<SharedPtr<BlockColumnInfo>, Status>
    GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id, Txn *txn);

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

    static Status CommitWrite(TableEntry *table_entry,
                              TransactionID txn_id,
                              TxnTimeStamp commit_ts,
                              const HashMap<SegmentID, TxnSegmentStore> &segment_stores,
                              const DeleteState *delete_state);

    static Status RollbackWrite(TableEntry *table_entry, TxnTimeStamp commit_ts, const Vector<TxnSegmentStore> &segment_stores);

    static SegmentID GetNextSegmentID(TableEntry *table_entry);

    // This not add row count
    static void AddSegment(TableEntry *table_entry, SharedPtr<SegmentEntry> &segment_entry);

public:
    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(Catalog *catalog, String function_name);

    static void AddFunctionSet(Catalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void AppendToScalarFunctionSet(Catalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void AddSpecialFunction(Catalog *catalog, const SharedPtr<SpecialFunction> &special_function);

    static Tuple<SpecialFunction *, Status> GetSpecialFunctionByNameNoExcept(Catalog *catalog, String function_name);

public:
    // Serialization and Deserialization
    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    void SaveFullCatalog(TxnTimeStamp max_commit_ts, String &full_path, String &full_name);

    bool SaveDeltaCatalog(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts, String &delta_path, String &delta_name);

    void AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    static UniquePtr<Catalog> NewCatalog();

    static UniquePtr<Catalog>
    LoadFromFiles(const FullCatalogFileInfo &full_ckp_info, const Vector<DeltaCatalogFileInfo> &delta_ckp_infos, BufferManager *buffer_mgr);

    SizeT GetDeltaLogCount() const;

    Vector<CatalogDeltaOpBrief> GetDeltaLogBriefs() const;

    static UniquePtr<nlohmann::json> LoadFullCheckpointToJson(const String& file_name);
    static UniquePtr<Catalog> LoadFullCheckpoint(const String &file_name);
    void AttachDeltaCheckpoint(const String &file_name);

    static UniquePtr<CatalogDeltaEntry> LoadFromFileDelta(const String &catalog_path);

private:
    static UniquePtr<Catalog> Deserialize(const nlohmann::json &catalog_json, BufferManager *buffer_mgr);

    void LoadFromEntryDelta(UniquePtr<CatalogDeltaEntry> delta_entry, BufferManager *buffer_mgr);

public:
    // Profile related methods

    void AppendProfileRecord(SharedPtr<QueryProfiler> profiler) { history_.Enqueue(std::move(profiler)); }

    const QueryProfiler *GetProfileRecord(SizeT index) { return history_.GetElement(index); }

    Vector<SharedPtr<QueryProfiler>> GetProfileRecords() { return history_.GetElements(); }

    void ResizeProfileHistory(SizeT new_size) { history_.Resize(new_size); }

    SizeT ProfileHistorySize() const { return history_.HistoryCapacity(); }

public:
    // Relative to the `data_dir` config item
    const SharedPtr<String> &CatalogDir() const { return catalog_dir_; }

    TransactionID next_txn_id() const { return next_txn_id_; }

    void SetProfile(bool flag) { enable_profile_ = flag; }
    [[nodiscard]] bool GetProfile() const { return enable_profile_; }

public:
    SharedPtr<String> catalog_dir_{};

    MetaMap<DBMeta> db_meta_map_{};

    Atomic<TransactionID> next_txn_id_{};

private:
    TxnTimeStamp full_ckp_commit_ts_{};

public:
    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_{};
    HashMap<String, SharedPtr<SpecialFunction>> special_functions_{};
    HashMap<String, UniquePtr<ColumnDef>> special_columns_{};

    ProfileHistory history_{DEFAULT_PROFILER_HISTORY_SIZE};
    atomic_bool enable_profile_{false};

private:
    Atomic<bool> running_{};
    UniquePtr<Thread> mem_index_commit_thread_{};

    void MemIndexCommit();

    void MemIndexCommitLoop();

public:
    void MemIndexRecover(BufferManager *buffer_manager, TxnTimeStamp ts);
    Vector<SharedPtr<SegmentIndexEntry>> all_memindex_recover_segment_;

    void PickCleanup(CleanupScanner *scanner);

    void InitCompactionAlg(TxnTimeStamp system_start_ts);

private:
    UniquePtr<GlobalCatalogDeltaEntry> global_catalog_delta_entry_{MakeUnique<GlobalCatalogDeltaEntry>()};
};

} // namespace infinity
