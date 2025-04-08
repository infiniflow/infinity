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

export module new_txn;

import stl;
import statement_common;
import meta_info;
import table_def;
import index_base;
import data_block;
import meta_state;
import buffer_manager;
import txn_state;
import new_txn_store;
import database_detail;
import status;
import extra_ddl_info;
import internal_types;
import value;
import snapshot_info;
import txn_context;
import column_def;
import extra_command;
import column_vector;
import buffer_handle;

namespace infinity {

class KVInstance;
class NewCatalog;
class NewTxnManager;
struct Catalog;
struct TableEntry;
struct DBEntry;
struct BaseEntry;
struct TableIndexEntry;
struct SegmentEntry;
struct WalEntry;
struct WalCmd;
struct WalCmdCreateDatabase;
struct WalCmdDropDatabase;
struct WalCmdCreateTable;
struct WalCmdDropTable;
struct WalCmdRenameTable;
struct WalCmdAddColumns;
struct WalCmdDropColumns;
struct WalCmdCreateIndex;
struct WalCmdDropIndex;
struct WalCmdImport;
struct WalCmdAppend;
struct WalCmdDelete;
struct WalCmdCompact;
struct WalCmdDumpIndex;
struct WalChunkIndexInfo;
struct WalSegmentInfo;
struct WalCmdCheckpoint;

class CatalogDeltaEntry;
class CatalogDeltaOperation;
class BaseTableRef;
enum class CompactStatementType;
struct SegmentIndexEntry;
struct AddDeltaEntryTask;
class BufferObj;

class ColumnMeta;
class BlockMeta;
class SegmentMeta;
class TableMeeta;
class ChunkIndexMeta;
class SegmentIndexMeta;
class TableIndexMeeta;
class DBMeeta;
struct MemIndex;

struct NewTxnCompactState;

struct AppendState;
struct AppendRange;
enum class DumpIndexCause;
struct IndexReader;

export struct CheckpointOption {
    TxnTimeStamp checkpoint_ts_ = 0;
};

export class NewTxn : public EnableSharedFromThis<NewTxn> {
public:
    // For new txn
    explicit NewTxn(NewTxnManager *txn_manager,
                    BufferManager *buffer_manager,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    UniquePtr<KVInstance> kv_instance,
                    SharedPtr<String> txn_text,
                    TransactionType txn_type);

    // For replay txn
    explicit NewTxn(BufferManager *buffer_mgr,
                    NewTxnManager *txn_mgr,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    UniquePtr<KVInstance> kv_instance,
                    TransactionType txn_type);

    virtual ~NewTxn();

    static UniquePtr<NewTxn> NewReplayTxn(BufferManager *buffer_mgr,
                                          NewTxnManager *txn_mgr,
                                          TransactionID txn_id,
                                          TxnTimeStamp begin_ts,
                                          UniquePtr<KVInstance> kv_instance,
                                          TransactionType txn_type);

    // NewTxn steps:
    // 1. CreateTxn
    // 2. Begin
    // 3. Commit() / Rollback
    // 3.1 PrepareCommit - multiple thread
    // 3.2 WriteWAL - single threads
    // 3.3 PrepareWriteData - single thread
    // 3.4 Commit - multiple threads

    //    void Begin();

    //    void SetBeginTS(TxnTimeStamp begin_ts);

    Status Commit();

    Status PostReadTxnCommit();

    bool CheckConflict1(NewTxn *check_txn, String &conflict_reason);

    Status PrepareCommit();

    void CommitBottom();

    void PostCommit();

    void CancelCommitBottom();

    Status PostRollback(TxnTimeStamp abort_ts);

    Status Rollback();

    // Database OPs
    Status CreateDatabase(const String &db_name, ConflictType conflict_type, const SharedPtr<String> &comment);

    Status DropDatabase(const String &db_name, ConflictType conflict_type);

    Tuple<DBEntry *, Status> GetDatabase(const String &db_name);

    // bool CheckDatabaseExists(const String &db_name);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name);

    Status ListDatabase(Vector<String> &db_names);

    // Table and Collection OPs
    Status GetTables(const String &db_name, Vector<TableDetail> &output_table_array);

    Status CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type);

    // Status RenameTable(TableEntry *old_table_entry, const String &new_table_name);

    Status AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs);

    Status DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names);

    // Status AddColumns(TableEntry *table_entry, const Vector<SharedPtr<ColumnDef>> &column_defs);

    // Status DropColumns(TableEntry *table_entry, const Vector<String> &column_names);

    Status CreateCollection(const String &db_name, const String &collection_name, ConflictType conflict_type, BaseEntry *&collection_entry);

    Status DropTable(const String &db_name, const String &table_name, ConflictType conflict_type);

    Status RenameTable(const String &db_name, const String &old_table_name, const String &new_table_name);

    Status LockTable(const String &db_name, const String &table_name);

    Status UnlockTable(const String &db_name, const String &table_name);

    Status ListTable(const String &db_name, Vector<String> &table_names);

    // Tuple<TableEntry *, Status> GetTableByName(const String &db_name, const String &table_name);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &db_name, const String &table_name);

    Status GetCollectionByName(const String &db_name, const String &table_name, BaseEntry *&collection_entry);

    Tuple<SharedPtr<TableSnapshotInfo>, Status> GetTableSnapshot(const String &db_name, const String &table_name);

    Status ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info);

    // Index OPs

    Status CreateIndex(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type);

    Status DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type);

    Status DumpMemIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id);

    Status OptimizeIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id);

    Status ListIndex(const String &db_name, const String &table_name, Vector<String> &index_names);

    // // If `prepare` is false, the index will be created in single thread. (called by `FsPhysicalCreateIndex`)
    // // Else, only data is stored in index (Called by `PhysicalCreateIndexPrepare`). And the index will be created by multiple threads in next
    // // operator. (called by `PhysicalCreateIndexDo`)
    // Tuple<TableIndexEntry *, Status> CreateIndexDef(TableEntry *table_entry, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type);

    // Tuple<TableIndexEntry *, Status> GetIndexByName(const String &db_name, const String &table_name, const String &index_name);

    // Tuple<SharedPtr<TableIndexInfo>, Status> GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name);

    // Pair<Vector<SegmentIndexEntry *>, Status>
    // CreateIndexPrepare(TableIndexEntry *table_index_entry, BaseTableRef *table_ref, bool prepare, bool check_ts = true);

    // Status CreateIndexDo(BaseTableRef *table_ref, const String &index_name, HashMap<SegmentID, atomic_u64> &create_index_idxes);

    // Status CreateIndexDo(TableEntry *table_entry,
    //                      const Map<SegmentID, SegmentIndexEntry *> &segment_index_entries,
    //                      const String &index_name,
    //                      HashMap<SegmentID, atomic_u64> &create_index_idxes);

    // Status CreateIndexFinish(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &indef);

    // Status CreateIndexFinish(const TableEntry *table_entry, const TableIndexEntry *table_index_entry);

    // View Ops
    // Fixme: view definition should be given
    Status CreateView(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status DropViewByName(const String &db_name, const String &view_name, ConflictType conflict_type, BaseEntry *&view_entry);

    Status GetViewByName(const String &db_name, const String &view_name, BaseEntry *&view_entry);

    Status GetViews(const String &db_name, Vector<ViewDetail> &output_view_array);

    // DML
    // Status Import(TableEntry *table_entry, SharedPtr<SegmentEntry> segment_entry);

    Status Import(const String &db_name, const String &table_name, const Vector<SharedPtr<DataBlock>> &input_blocks);

private:
    Status ReplayImport(WalCmdImport *import_cmd);

public:
    // Status Append(TableEntry *table_entry, const SharedPtr<DataBlock> &input_block);

    Status Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block);

    Status Append(const TableInfo &table_info, const SharedPtr<DataBlock> &input_block);

private:
    Status AppendInner(const String &db_name,
                       const String &table_name,
                       const String &table_key,
                       TableMeeta &table_meta,
                       const SharedPtr<DataBlock> &input_block);

public:
    // Status Delete(TableEntry *table_entry, const Vector<RowID> &row_ids, bool check_conflict = true);

    Status Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids);

    Status Compact(const String &db_name, const String &table_name);

    Status CheckTableIfDelete(const String &db_name, const String &table_name, bool &has_delete);

private:
    Status ReplayCompact(WalCmdCompact *compact_cmd);

public:
    // Status Compact(TableEntry *table_entry, Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType
    // type);

    // Status OptIndex(TableIndexEntry *table_index_entry, Vector<UniquePtr<InitParameter>> init_params);

    Status Checkpoint();

    // Getter
    BufferManager *buffer_mgr() const { return buffer_mgr_; }

    inline TransactionID TxnID() const { return txn_context_ptr_->txn_id_; }

    TxnTimeStamp CommitTS() const;

    TxnTimeStamp BeginTS() const;

    TxnState GetTxnState() const;

    TransactionType GetTxnType() const;

    bool IsWriteTransaction() const;

    void SetTxnCommitted();

    void SetTxnCommitting(TxnTimeStamp commit_ts);

    void SetTxnRollbacking(TxnTimeStamp rollback_ts);

    void SetTxnRollbacked();

    void SetTxnRead();

    void SetTxnWrite();

    void FullCheckpoint(const TxnTimeStamp max_commit_ts);

    bool DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts);

    NewTxnManager *txn_mgr() const { return txn_mgr_; }

    // Create txn store if not exists
    NewTxnTableStore *GetNewTxnTableStore(const String &table_name);

    NewTxnTableStore *GetExistNewTxnTableStore(TableEntry *table_entry) const;

    WalEntry *GetWALEntry() const;

    const SharedPtr<String> GetTxnText() const { return txn_text_; }

    const String &db_name() const { return db_name_; }

    void SetDBName(const String &db_name) { db_name_ = db_name; }

    void AddWriteTxnNum(TableEntry *table_entry);

    // Some transaction need to pass the txn access right check in txn commit phase;
    void SetReaderAllowed(bool allowed) { allowed_in_reader_ = allowed; }

    bool IsReaderAllowed() const { return allowed_in_reader_; }

    NewTxnStore *txn_store() { return &txn_store_; }

    SharedPtr<TxnContext> txn_context() const { return txn_context_ptr_; }
    void AddOperation(const SharedPtr<String> &operation_text) { txn_context_ptr_->AddOperation(operation_text); }
    Vector<SharedPtr<String>> GetOperations() const { return txn_context_ptr_->GetOperations(); }

    KVInstance *kv_instance() const { return kv_instance_.get(); }

    Status PrintVersion(const String &db_name, const String &table_name, const Vector<RowID> &row_ids, bool ignore_invisible);

private:
    void CheckTxnStatus();

    void CheckTxn(const String &db_name);

public:
    Status GetDBMeta(const String &db_name, Optional<DBMeeta> &db_meta, String *db_key = nullptr);

    Status GetTableMeta(const String &db_name,
                        const String &table_name,
                        Optional<DBMeeta> &db_meta,
                        Optional<TableMeeta> &table_meta,
                        String *table_key = nullptr);

    Status GetTableMeta(const String &table_name, DBMeeta &db_meta, Optional<TableMeeta> &table_meta, String *table_key = nullptr);

    Status GetTableIndexMeta(const String &db_name,
                             const String &table_name,
                             const String &index_name,
                             Optional<DBMeeta> &db_meta,
                             Optional<TableMeeta> &table_meta,
                             Optional<TableIndexMeeta> &table_index_meta,
                             String *table_key,
                             String *index_key);

    Status
    GetTableIndexMeta(const String &index_name, TableMeeta &table_meta, Optional<TableIndexMeeta> &table_index_meta, String *index_key = nullptr);

private:
    friend struct NewTxnCompactState;

    // DML

    Status PrepareAppendInBlock(BlockMeta &block_meta, AppendState *append_state, bool &block_full, bool &segment_full);

    Status AppendInBlock(BlockMeta &block_meta, SizeT block_offset, SizeT append_rows, const DataBlock *input_block, SizeT input_offset);

    Status AppendInColumn(ColumnMeta &column_meta, SizeT dest_offset, SizeT append_rows, const ColumnVector &column_vector, SizeT source_offset);

    Status DeleteInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets, Vector<BlockOffset> &undo_block_offsets);

    Status RollbackDeleteInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets);

    Status PrintVersionInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets, bool ignore_invisible);

    Status CompactBlock(BlockMeta &block_meta, NewTxnCompactState &compact_state);

    Status AddColumnsData(TableMeeta &table_meta, const Vector<SharedPtr<ColumnDef>> &column_defs);

    Status AddColumnsDataInSegment(SegmentMeta &segment_meta, const Vector<SharedPtr<ColumnDef>> &column_defs, const Vector<Value> &default_values);

    Status AddColumnsDataInBlock(BlockMeta &block_meta, const Vector<SharedPtr<ColumnDef>> &column_defs, const Vector<Value> &default_values);

    Status DropColumnsData(TableMeeta &table_meta, const Vector<ColumnID> &column_ids);

    Status AppendIndex(TableIndexMeeta &table_index_meta, const Vector<AppendRange> &append_ranges);

    Status AppendMemIndex(SegmentIndexMeta &segment_index_meta, RowID base_row_id, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt);

    Status PopulateIndex(const String &db_name,
                         const String &table_name,
                         const String &index_name,
                         const String &table_key,
                         TableIndexMeeta &table_index_meta,
                         SegmentMeta &segment_meta,
                         SizeT segment_row_cnt,
                         DumpIndexCause dump_index_cause,
                         WalCmdCreateIndex *create_index_cmd_ptr = nullptr);

    Status ReplayDumpIndex(WalCmdDumpIndex *dump_index_cmd);

    Status PopulateIndexToMem(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, ColumnID column_id, SizeT segment_row_cnt);

    Status PopulateFtIndexInner(SharedPtr<IndexBase> index_base,
                                SegmentIndexMeta &segment_index_meta,
                                SegmentMeta &segment_meta,
                                ColumnID column_id,
                                SizeT segment_row_cnt);

    Status PopulateIvfIndexInner(SharedPtr<IndexBase> index_base,
                                 SegmentIndexMeta &segment_index_meta,
                                 SegmentMeta &segment_meta,
                                 SharedPtr<ColumnDef> column_def,
                                 ChunkID &new_chunk_id);

    Status PopulateEmvbIndexInner(SharedPtr<IndexBase> index_base,
                                  SegmentIndexMeta &segment_index_meta,
                                  SegmentMeta &segment_meta,
                                  SharedPtr<ColumnDef> column_def,
                                  ChunkID &new_chunk_id);

    Status OptimizeFtIndex(SharedPtr<IndexBase> index_base, SegmentIndexMeta &segment_index_meta, RowID &base_rowid, u32 &row_cnt, String &base_name);

    Status OptimizeVecIndex(SharedPtr<IndexBase> index_base,
                            SharedPtr<ColumnDef> column_def,
                            SegmentMeta &segment_meta,
                            RowID base_rowid,
                            u32 row_cnt,
                            BufferObj *buffer_obj);

    Status DumpMemIndexInner(SegmentIndexMeta &segment_index_meta, ChunkID &new_chunk_id);

    Status AddChunkWal(const String &db_name,
                       const String &table_name,
                       const String &index_name,
                       const String &table_key,
                       ChunkIndexMeta &chunk_index_meta,
                       const Vector<ChunkID> &deprecate_ids,
                       DumpIndexCause dump_index_cause);

    Status CheckpointDB(DBMeeta &db_meta, const CheckpointOption &option);

    Status CheckpointTable(TableMeeta &table_meta, const CheckpointOption &option);

    Status CheckpointTableData(TableMeeta &table_meta, const CheckpointOption &option);

    Status CountMemIndexGapInSegment(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, Vector<AppendRange> &append_ranges);

public:
    Status RecoverMemIndex(TableIndexMeeta &table_index_meta);

    static Status CommitMemIndex(TableIndexMeeta &table_index_meta);

    Status GetFullTextIndexReader(const String &db_name, const String &table_name, SharedPtr<IndexReader> &index_reader);

private:
    Status CommitCreateDB(const WalCmdCreateDatabase *create_db_cmd);
    Status CommitDropDB(const WalCmdDropDatabase *drop_db_cmd);
    Status CommitCreateTable(const WalCmdCreateTable *create_table_cmd);
    Status CommitDropTable(const WalCmdDropTable *drop_table_cmd);
    Status CommitRenameTable(const WalCmdRenameTable *create_table_cmd);
    Status CommitAddColumns(const WalCmdAddColumns *add_columns_cmd);
    Status CommitDropColumns(const WalCmdDropColumns *drop_columns_cmd);
    Status CommitCreateIndex(WalCmdCreateIndex *create_index_cmd);
    Status CommitDropIndex(const WalCmdDropIndex *drop_index_cmd);
    Status CommitImport(WalCmdImport *import_cmd);
    Status CommitAppend(const WalCmdAppend *append_cmd, KVInstance *kv_instance);
    Status PostCommitAppend(const WalCmdAppend *append_cmd, KVInstance *kv_instance);
    Status PrepareCommitDelete(const WalCmdDelete *delete_cmd, KVInstance *kv_instance);
    Status RollbackDelete(const WalCmdDelete *delete_cmd, KVInstance *kv_instance);
    Status CommitCompact(WalCmdCompact *compact_cmd);
    Status PostCommitDumpIndex(const WalCmdDumpIndex *dump_index_cmd, KVInstance *kv_instance);
    Status CommitCheckpoint(const WalCmdCheckpoint *checkpoint_cmd);
    Status CommitCheckpointDB(DBMeeta &db_meta, const WalCmdCheckpoint *checkpoint_cmd);
    Status CommitCheckpointTable(TableMeeta &table_meta, const WalCmdCheckpoint *checkpoint_cmd);
    Status CommitCheckpointTableData(TableMeeta &table_meta, TxnTimeStamp checkpoint_ts);

    Status CommitSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta);
    Status FlushVersionFile(BlockMeta &block_meta, TxnTimeStamp save_ts);
    Status FlushColumnFiles(BlockMeta &block_meta, TxnTimeStamp save_ts);
    Status TryToMmap(BlockMeta &block_meta, TxnTimeStamp save_ts, bool *to_mmap = nullptr);

    Status IncrLatestID(String &id_str, std::string_view id_name) const;

    // Check transaction conflicts
    bool CheckConflictWithAppend(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause);
    bool CheckConflictWithImport(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause);
    bool CheckConflictWithCompact(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause);
    bool CheckConflictWithCreateIndex(const String &db_name, const String &table_name, NewTxn *previous_txn, String &cause);

public:
    static Status Cleanup(TxnTimeStamp ts, KVInstance *kv_instance);

    void SetReplay(bool replay) { txn_context_ptr_->replay_ = replay; }

    bool IsReplay() const { return txn_context_ptr_->replay_; }

    Status ReplayWalCmd(const SharedPtr<WalCmd> &wal_cmd);

    Status GetDBFilePaths(const String &db_name, Vector<String> &file_paths);
    Status GetTableFilePaths(const String &db_name, const String &table_name, Vector<String> &file_paths);
    Status GetSegmentFilePaths(const String &db_name, const String &table_name, SegmentID segment_id, Vector<String> &file_paths);
    Status GetBlockFilePaths(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, Vector<String> &file_paths);
    Status GetBlockColumnFilePaths(const String &db_name,
                                   const String &table_name,
                                   SegmentID segment_id,
                                   BlockID block_id,
                                   ColumnID column_id,
                                   Vector<String> &file_paths);
    Status GetColumnFilePaths(const String &db_name, const String &table_name, const String &column_name, Vector<String> &file_paths);
    Status GetTableIndexFilePaths(const String &db_name, const String &table_name, const String &index_name, Vector<String> &file_paths);
    Status GetSegmentIndexFilepaths(const String &db_name,
                                    const String &table_name,
                                    const String &index_name,
                                    SegmentID segment_id,
                                    Vector<String> &file_paths);
    Status GetChunkIndexFilePaths(const String &db_name,
                                  const String &table_name,
                                  const String &index_name,
                                  SegmentID segment_id,
                                  ChunkID chunk_id,
                                  Vector<String> &file_paths);

    void IncreaseTableReferenceCount(const String &table_key);
    void DecreaseTableReferenceCount(const String &table_key);
    SizeT GetTableReferenceCount(const String &table_key);

    Status Dummy();

private:
    // Reference to external class
    NewTxnManager *txn_mgr_{};
    BufferManager *buffer_mgr_{}; // This BufferManager ptr Only for replaying wal
    Catalog *catalog_{};
    NewCatalog *new_catalog_{};

    NewTxnStore txn_store_; // this has this ptr, so txn cannot be moved.

    // Use as txn context;
    mutable std::shared_mutex rw_locker_{};

    // Handled database
    String db_name_{};

    /// LOG
    // WalEntry
    SharedPtr<WalEntry> wal_entry_{};

    Vector<SharedPtr<BaseExtraCommand>> extra_commands_{};
    // TODO: remove this
    UniquePtr<CatalogDeltaEntry> txn_delta_ops_entry_{};

    // WalManager notify the commit bottom half is done
    std::mutex commit_lock_{};
    std::condition_variable commit_cv_{};
    bool commit_bottom_done_{false};

    // KV txn instance
    UniquePtr<KVInstance> kv_instance_{};

    // String
    SharedPtr<String> txn_text_{nullptr};

    // ADMIN command which allowed in follower and learner
    bool allowed_in_reader_{false};

    HashMap<String, SizeT> table_reference_count_{};

private:
    SharedPtr<TxnContext> txn_context_ptr_{};

    friend class NewTxnTableStore;
};

} // namespace infinity
