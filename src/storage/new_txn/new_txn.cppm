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

export module infinity_core:new_txn;

import :stl;
import statement_common;
import :txn_state;
import :new_txn_store;
import :database_detail;
import :status;
import extra_ddl_info;
import internal_types;
import :value;
import :snapshot_info;
import column_def;
import :column_vector;
import :fast_rough_filter;


namespace infinity {



class KVInstance;
class NewCatalog;
class NewTxnManager;
struct WalEntry;
struct WalCmd;
struct WalCmdCreateDatabaseV2;
struct WalCmdDropDatabaseV2;
struct WalCmdCreateTableV2;
struct WalCmdDropTableV2;
struct WalCmdRenameTableV2;
struct WalCmdAddColumnsV2;
struct WalCmdDropColumnsV2;
struct WalCmdCreateIndexV2;
struct WalCmdDropIndexV2;
struct WalCmdImportV2;
struct WalCmdAppendV2;
struct WalCmdDeleteV2;
struct WalCmdCompactV2;
struct WalCmdDumpIndexV2;
struct WalChunkIndexInfo;
struct WalSegmentInfo;
struct WalCmdCheckpointV2;
struct WalCmdOptimizeV2;
struct WalCmdCleanup;
struct WalCmdCreateTableSnapshot;
struct WalCmdRestoreTableSnapshot;
struct WalCmdRestoreDatabaseSnapshot;

class BufferObj;

class ColumnMeta;
class BlockMeta;
class SegmentMeta;
class TableMeeta;
class ChunkIndexMeta;
class ChunkIndexMetaInfo;
class SegmentIndexMeta;
class TableIndexMeeta;
class DBMeeta;
struct MemIndex;

struct NewTxnCompactState;

struct AppendState;
enum class DumpIndexCause;
struct IndexReader;

struct BaseTxnStore;
struct CreateDBTxnStore;
struct DropDBTxnStore;
struct CreateTableTxnStore;
struct AppendTxnStore;
struct ImportTxnStore;
struct AddColumnsTxnStore;
struct DropColumnsTxnStore;
struct CompactTxnStore;
struct CreateIndexTxnStore;
struct DropIndexTxnStore;
struct OptimizeIndexTxnStore;
struct DumpMemIndexTxnStore;
struct DeleteTxnStore;
struct DropTableTxnStore;
struct RenameTableTxnStore;
struct RestoreTableTxnStore;
struct RestoreDatabaseTxnStore;
struct UpdateTxnStore;
struct CreateTableSnapshotTxnStore;
class BufferManager;
class IndexBase;
struct DataBlock;
class TableDef;
struct TxnContext;
struct TableInfo;
struct DatabaseInfo;
struct TableIndexInfo;
struct SegmentIndexInfo;
struct SegmentInfo;
struct BlockInfo;
struct BlockColumnInfo;
struct TableDetail;
struct CheckpointTxnStore;
struct MetaKey;

export struct CheckpointOption {
    TxnTimeStamp checkpoint_ts_ = 0;
};

export struct ChunkInfoForCreateIndex {
    String db_id_{};
    String table_id_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
};

enum class TxnType {
    kReadOnly,
    kWrite,
    kInvalid,
};

export class NewTxn : public EnableSharedFromThis<NewTxn> {
public:
    // For new txn
    explicit NewTxn(NewTxnManager *txn_manager,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    TxnTimeStamp last_kv_commit_ts,
                    TxnTimeStamp last_commit_ts,
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

    static UniquePtr<NewTxn>
    NewReplayTxn(NewTxnManager *txn_mgr, TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, UniquePtr<KVInstance> kv_instance);

    static UniquePtr<NewTxn> NewRecoveryTxn(NewTxnManager *txn_mgr, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

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

    Status CommitReplay();

    Status CommitRecovery();

    bool CheckConflict1(SharedPtr<NewTxn> check_txn, String &conflict_reason, bool &retry_query);
    bool CheckConflictTxnStores(SharedPtr<NewTxn> check_txn, String &conflict_reason, bool &retry_query);

    Status PrepareCommit();

    void CommitBottom();

    void NotifyTopHalf();

    void PostCommit();

    void CancelCommitBottom();

    Status PostRollback(TxnTimeStamp abort_ts);

    Status Rollback();

    // Database OPs
    Status CreateDatabase(const String &db_name, ConflictType conflict_type, const SharedPtr<String> &comment);

    Status DropDatabase(const String &db_name, ConflictType conflict_type);

    // bool CheckDatabaseExists(const String &db_name);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name);

    Status ListDatabase(Vector<String> &db_names);

    // Table and Collection OPs
    Status GetTables(const String &db_name, Vector<SharedPtr<TableDetail>> &output_table_array);

    Status CreateTable(const String &db_name, const SharedPtr<TableDef> &table_def, ConflictType conflict_type);

    Status AddColumns(const String &db_name, const String &table_name, const Vector<SharedPtr<ColumnDef>> &column_defs);

    Status DropColumns(const String &db_name, const String &table_name, const Vector<String> &column_names);

    Status DropTable(const String &db_name, const String &table_name, ConflictType conflict_type);

    Status RenameTable(const String &db_name, const String &old_table_name, const String &new_table_name);

    Status ListTable(const String &db_name, Vector<String> &table_names);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(const String &db_name, const String &table_name);

    Tuple<SharedPtr<TableIndexInfo>, Status> GetTableIndexInfo(const String &db_name, const String &table_name, const String &index_name);

    Tuple<SharedPtr<SegmentIndexInfo>, Status>
    GetSegmentIndexInfo(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id);

    Tuple<SharedPtr<ChunkIndexMetaInfo>, Status>
    GetChunkIndexInfo(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id, ChunkID chunk_id);

    Tuple<SharedPtr<SegmentInfo>, Status> GetSegmentInfo(const String &db_name, const String &table_name, SegmentID segment_id);

    Tuple<Vector<SharedPtr<SegmentInfo>>, Status> GetSegmentsInfo(const String &db_name, const String &table_name);

    Tuple<SharedPtr<BlockInfo>, Status> GetBlockInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id);

    Tuple<Vector<SharedPtr<BlockInfo>>, Status> GetBlocksInfo(const String &db_name, const String &table_name, SegmentID segment_id);

    Tuple<SharedPtr<BlockColumnInfo>, Status>
    GetBlockColumnInfo(const String &db_name, const String &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id);

    // Tuple<SharedPtr<TableSnapshotInfo>, Status> GetTableSnapshot(const String &db_name, const String &table_name);

    // Status ApplyTableSnapshot(const SharedPtr<TableSnapshotInfo> &table_snapshot_info);

    // Index OPs

    Status CreateIndex(const String &db_name, const String &table_name, const SharedPtr<IndexBase> &index_base, ConflictType conflict_type);

    Status DropIndexByName(const String &db_name, const String &table_name, const String &index_name, ConflictType conflict_type);

    Status DumpMemIndex(const String &db_name, const String &table_name, const String &index_name);

    Status
    DumpMemIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id, RowID begin_row_id = RowID());

    Status OptimizeAllIndexes();

    Status OptimizeTableIndexes(const String &db_name, const String &table_name);

    Status OptimizeIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id);


    // // Snapshot OPs
    Status CreateTableSnapshot(const String &db_name, const String &table_name, const String &snapshot_name);

    // Tuple<SharedPtr<TableSnapshotInfo>, Status> GetTableSnapshotInfo(const String &db_name, const String &table_name);

    Status RestoreTableSnapshot(const String &db_name, const SharedPtr<TableSnapshotInfo> &table_snapshot_info);


    Status RestoreTableIndexesFromSnapshot(TableMeeta &table_meta, const Vector<WalCmdCreateIndexV2> &index_cmds, bool is_link_files = false);

    Status DropTableSnapShot(const String &db_name, const String &table_name);

    Tuple<SharedPtr<DatabaseSnapshotInfo>, Status> GetDatabaseSnapshotInfo(const String &db_name);

    Status RestoreDatabaseSnapshot(const SharedPtr<DatabaseSnapshotInfo> &database_snapshot_info);
  
    friend class NewTxnManager;


private:
    Status OptimizeIndexInner(SegmentIndexMeta &segment_index_meta,
                              const String &index_name,
                              const String &table_name,
                              const String &db_name,
                              const String &table_key);

public:
    Status OptimizeIndexByParams(const String &db_name, const String &table_name, const String &index_name, Vector<UniquePtr<InitParameter>> params);

    Status ListIndex(const String &db_name, const String &table_name, Vector<String> &index_names);

    Status Import(const String &db_name, const String &table_name, const Vector<SharedPtr<DataBlock>> &input_blocks);

private:
    Status ReplayCreateDb(WalCmdCreateDatabaseV2 *create_db_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayDropDb(WalCmdDropDatabaseV2 *drop_db_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayCreateTable(WalCmdCreateTableV2 *create_table_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayDropTable(WalCmdDropTableV2 *drop_table_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayRenameTable(WalCmdRenameTableV2 *rename_table_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayAddColumns(WalCmdAddColumnsV2 *add_columns_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayDropColumns(WalCmdDropColumnsV2 *drop_columns_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayCreateIndex(WalCmdCreateIndexV2 *create_index_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayDumpIndex(WalCmdDumpIndexV2 *dump_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayDropIndex(WalCmdDropIndexV2 *drop_index_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayAppend(WalCmdAppendV2 *append_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayDelete(WalCmdDeleteV2 *delete_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayImport(WalCmdImportV2 *import_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayCompact(WalCmdCompactV2 *compact_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayOptimize(WalCmdOptimizeV2 *optimize_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayCheckpoint(WalCmdCheckpointV2 *optimize_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayCleanup(WalCmdCleanup *cleanup_cmd, TxnTimeStamp commit_ts, i64 txn_id);
    Status ReplayRestoreTableSnapshot(WalCmdRestoreTableSnapshot *restore_table_cmd, TxnTimeStamp commit_ts, i64 txn_id);

public:
    Status Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block);

    Status Append(const TableInfo &table_info, const SharedPtr<DataBlock> &input_block);

    Status Update(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block, const Vector<RowID> &row_ids);

private:
    Tuple<Vector<Pair<RowID, u64>>, Status> GetRowRanges(TableMeeta &table_meta, const SharedPtr<DataBlock> &input_block);

    Status AppendInner(const String &db_name,
                       const String &table_name,
                       const String &table_key,
                       TableMeeta &table_meta,
                       const SharedPtr<DataBlock> &input_block);

    Status DeleteInner(const String &db_name, const String &table_name, TableMeeta &table_meta, const Vector<RowID> &row_ids);

public:
    Status Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids);

    Status Compact(const String &db_name, const String &table_name, const Vector<SegmentID> &segment_ids);

    Status CheckTableIfDelete(TableMeeta &table_meta, bool &has_delete);

    Status Cleanup();

private:
    Status ReplayCompact(WalCmdCompactV2 *compact_cmd);

    Status CleanupInner(const Vector<UniquePtr<MetaKey>> &metas);

public:
    Status Checkpoint(TxnTimeStamp last_ckp_ts);

    // Getter
    BufferManager *buffer_mgr() const { return buffer_mgr_; }

    TransactionID TxnID() const;

    TxnTimeStamp BeginTS() const;

    TxnTimeStamp CommitTS() const;

    TxnTimeStamp KVCommitTS() const;

    TxnTimeStamp LastSystemKVCommitTS() const;

    TxnTimeStamp LastSystemCommitTS() const;

    void SetTxnKVCommitTS(TxnTimeStamp kv_commit_ts);

    TxnState GetTxnState() const;

    TransactionType GetTxnType() const;

    bool NeedToAllocate() const;

    void SetTxnType(TransactionType type);

    bool IsWriteTransaction() const;

    void SetTxnCommitted();

    void SetTxnCommitting(TxnTimeStamp commit_ts);

    void SetTxnRollbacking(TxnTimeStamp rollback_ts);

    void SetTxnRollbacked();

    void SetTxnBottomDone();
    bool GetTxnBottomDone();

    void SetTxnRead();

    void SetTxnWrite();

    void FullCheckpoint(const TxnTimeStamp max_commit_ts);

    bool DeltaCheckpoint(TxnTimeStamp last_ckp_ts, TxnTimeStamp &max_commit_ts);

    NewTxnManager *txn_mgr() const { return txn_mgr_; }

    WalEntry *GetWALEntry() const;

    const SharedPtr<String> GetTxnText() const { return txn_text_; }

    const String &db_name() const { return db_name_; }

    void SetDBName(const String &db_name) { db_name_ = db_name; }

    // Some transaction need to pass the txn access right check in txn commit phase;
    void SetReaderAllowed(bool allowed) { allowed_in_reader_ = allowed; }

    bool IsReaderAllowed() const { return allowed_in_reader_; }

    BaseTxnStore *GetTxnStore() const { return base_txn_store_.get(); }

    SharedPtr<TxnContext> txn_context() const { return txn_context_ptr_; }
    void AddOperation(const SharedPtr<String> &operation_text);
    Vector<SharedPtr<String>> GetOperations() const;

    KVInstance *kv_instance() const { return kv_instance_.get(); }

    void AddMetaKeyForCommit(const String &key);

    TxnTimeStamp GetCurrentCkpTS() const;

private:
    void CheckTxnStatus();

    void CheckTxn(const String &db_name);

public:
    Status GetDBMeta(const String &db_name, SharedPtr<DBMeeta> &db_meta_ptr, TxnTimeStamp &db_create_ts, String *db_key = nullptr);

    Status GetTableMeta(const String &db_name,
                        const String &table_name,
                        SharedPtr<DBMeeta> &db_meta,
                        Optional<TableMeeta> &table_meta,
                        String *table_key = nullptr);

    Status GetTableMeta(const String &table_name, DBMeeta &db_meta, Optional<TableMeeta> &table_meta, String *table_key = nullptr);

    Status GetTableIndexMeta(const String &db_name,
                             const String &table_name,
                             const String &index_name,
                             SharedPtr<DBMeeta> &db_meta,
                             Optional<TableMeeta> &table_meta,
                             Optional<TableIndexMeeta> &table_index_meta,
                             String *table_key,
                             String *index_key);

    Status
    GetTableIndexMeta(const String &index_name, TableMeeta &table_meta, Optional<TableIndexMeeta> &table_index_meta, String *index_key = nullptr);

private:
    friend struct NewTxnCompactState;

    // DML
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

    Status AppendIndex(TableIndexMeeta &table_index_meta, const Pair<RowID, u64> &append_range);

    Status AppendMemIndex(SegmentIndexMeta &segment_index_meta, BlockID block_id, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt);

    Status PopulateIndex(const String &db_name,
                         const String &table_name,
                         const String &index_name,
                         const String &table_key,
                         TableIndexMeeta &table_index_meta,
                         SegmentMeta &segment_meta,
                         SizeT segment_row_cnt,
                         DumpIndexCause dump_index_cause,
                         WalCmdCreateIndexV2 *create_index_cmd_ptr = nullptr);

    Status ReplayDumpIndex(WalCmdDumpIndexV2 *dump_index_cmd);

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

    Status OptimizeSegmentIndexByParams(SegmentIndexMeta &segment_index_meta, const Vector<UniquePtr<InitParameter>> &params);

    Status ReplayOptimizeIndeByParams(WalCmdOptimizeV2 *optimize_cmd);

    Status DumpSegmentMemIndex(SegmentIndexMeta &segment_index_meta, const ChunkID &new_chunk_id);

    Status CheckpointDB(DBMeeta &db_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store);

    Status CheckpointTable(TableMeeta &table_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store);

    Status CountMemIndexGapInSegment(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, Vector<Pair<RowID, u64>> &append_ranges);

public:
    Status RecoverMemIndex(TableIndexMeeta &table_index_meta);

    Status CommitMemIndex(TableIndexMeeta &table_index_meta);

    Status GetFullTextIndexReader(const String &db_name, const String &table_name, SharedPtr<IndexReader> &index_reader);


private:
    Status PrepareCommitCreateDB(const WalCmdCreateDatabaseV2 *create_db_cmd);
    Status PrepareCommitDropDB(const WalCmdDropDatabaseV2 *drop_db_cmd);
    Status PrepareCommitCreateTable(const WalCmdCreateTableV2 *create_table_cmd);
    Status PrepareCommitDropTable(const WalCmdDropTableV2 *drop_table_cmd);
    Status PrepareCommitRenameTable(const WalCmdRenameTableV2 *create_table_cmd);
    Status PrepareCommitAddColumns(const WalCmdAddColumnsV2 *add_columns_cmd);
    Status PrepareCommitDropColumns(const WalCmdDropColumnsV2 *drop_columns_cmd);
    Status PrepareCommitCreateIndex(WalCmdCreateIndexV2 *create_index_cmd);
    Status PrepareCommitDropIndex(const WalCmdDropIndexV2 *drop_index_cmd);
    Status PrepareCommitImport(WalCmdImportV2 *import_cmd);
    Status CommitBottomAppend(WalCmdAppendV2 *append_cmd);
    Status CommitBottomDumpMemIndex(WalCmdDumpIndexV2 *dump_index_cmd);
    Status PrepareCommitDelete(const WalCmdDeleteV2 *delete_cmd);
    Status RollbackDelete(const DeleteTxnStore *delete_txn_store);
    Status PrepareCommitCompact(WalCmdCompactV2 *compact_cmd);
    Status PrepareCommitDumpIndex(const WalCmdDumpIndexV2 *dump_index_cmd, KVInstance *kv_instance);
    Status PrepareCommitCheckpoint(const WalCmdCheckpointV2 *checkpoint_cmd);
    Status CommitCheckpointDB(DBMeeta &db_meta, const WalCmdCheckpointV2 *checkpoint_cmd);
    Status CommitCheckpointTable(TableMeeta &table_meta, const WalCmdCheckpointV2 *checkpoint_cmd);
    Status CommitCheckpointTableData(TableMeeta &table_meta, TxnTimeStamp checkpoint_ts);
    Status PrepareCommitCreateTableSnapshot(const WalCmdCreateTableSnapshot *create_table_snapshot_cmd);
    Status PrepareCommitRestoreTableSnapshot(const WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, bool is_link_files = false);
    Status PrepareCommitRestoreDatabaseSnapshot(const WalCmdRestoreDatabaseSnapshot *restore_database_snapshot_cmd);
    Status CommitBottomCreateTableSnapshot(WalCmdCreateTableSnapshot *create_table_snapshot_cmd);
    Status CheckpointInner(TxnTimeStamp last_ckp_ts, CheckpointTxnStore *txn_store);

    Status AddSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta);
    Status CommitSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta);
    Status FlushVersionFile(BlockMeta &block_meta, TxnTimeStamp save_ts);
    Status FlushColumnFiles(BlockMeta &block_meta, TxnTimeStamp save_ts);
    Status TryToMmap(BlockMeta &block_meta,
                     TxnTimeStamp save_ts,
                     KVInstance *kv_instance,
                     TxnTimeStamp begin_ts,
                     TxnTimeStamp commit_ts,
                     bool *to_mmap = nullptr);

    Status IncrLatestID(String &id_str, std::string_view id_name) const;

    // Check transaction conflicts
    bool CheckConflictCmd(const WalCmd &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdCreateDatabaseV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdDropDatabaseV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdCreateTableV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdAppendV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdImportV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdAddColumnsV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdDropColumnsV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdCompactV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdCreateIndexV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdDumpIndexV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdDeleteV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictCmd(const WalCmdDropTableV2 &cmd, NewTxn *previous_txn, String &cause, bool &retry_query);

    bool CheckConflictTxnStore(NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateDBTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropDBTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const AppendTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const ImportTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const AddColumnsTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropColumnsTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CompactTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const OptimizeIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DumpMemIndexTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DeleteTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const RenameTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const UpdateTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const RestoreTableTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const RestoreDatabaseTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateTableSnapshotTxnStore &txn_store, NewTxn *previous_txn, String &cause, bool &retry_query);

public:
    bool IsReplay() const;

    Status ReplayWalCmd(const SharedPtr<WalCmd> &wal_cmd, TxnTimeStamp commit_ts, i64 txn_id);

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
    Status ProcessSnapshotRestorationData(const String &db_name,
                                             const String &db_id_str,
                                             const String &table_name,
                                             const String &table_id_str,
                                             const SharedPtr<TableDef> &table_def,
                                             const SharedPtr<TableSnapshotInfo> &table_snapshot_info,
                                             const String &snapshot_name,
                                             RestoreTableTxnStore *txn_store);
    Status RestoreTableFromSnapshot(const WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, DBMeeta &db_meta, bool is_link_files = false);
    Status ManualDumpIndex(const String &db_name, const String &table_name);



    Status Dummy();
    void SetWalSize(i64 wal_size);

    // Get the table id which is used in the txn. Return empty string if no table is used.
    String GetTableIdStr();

    void AddSemaphore(UniquePtr<std::binary_semaphore> sema);
    const Vector<UniquePtr<std::binary_semaphore>> &semas() const;
    void AddMetaKeyForBufferObject(UniquePtr<MetaKey> object_meta_key);

private:
    // Reference to external class
    NewTxnManager *txn_mgr_{};
    BufferManager *buffer_mgr_{}; // This BufferManager ptr Only for replaying wal
    NewCatalog *new_catalog_{};

    // Used to store the local data in this transaction
    SharedPtr<BaseTxnStore> base_txn_store_{};

    NewTxnStore txn_store_; // this has this ptr, so txn cannot be moved.
    TxnType txn_type_{TxnType::kInvalid};

    // Use as txn context;
    mutable std::shared_mutex rw_locker_{};

    // Handled database
    String db_name_{};

    /// LOG
    // WalEntry
    SharedPtr<WalEntry> wal_entry_{};

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

    Vector<String> keys_wait_for_commit_{};

    // Used for new checkpoint
    TxnTimeStamp current_ckp_ts_{};
    SizeT wal_size_{};

    // Use for commit and rollback
    Vector<UniquePtr<std::binary_semaphore>> semas_{};

private:
    SharedPtr<TxnContext> txn_context_ptr_{};

    friend class NewTxnTableStore;

private:
    void WaitForCompletion();
    void SetCompletion();

public:
    SharedPtr<NewTxn> conflicted_txn_{};
    std::mutex finished_mutex_{};
    std::condition_variable finished_cv_{};
    bool finished_{false};

private:
    Vector<ChunkInfoForCreateIndex> chunk_infos_; // For cleanup when create index is rollbacked

    bool bottom_done_{false}; // TODO: Use a transaction state instead of a bool flag
};

} // namespace infinity