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

import :txn_state;
import :new_txn_store;
import :database_detail;
import :status;
import :value;
import :snapshot_info;
import :column_vector;
import :fast_rough_filter;
import :txn_context;

import std;

import column_def;
import extra_ddl_info;
import internal_types;
import statement_common;

namespace infinity {

class KVInstance;

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
class TableMeta;
class ChunkIndexMeta;
class ChunkIndexMetaInfo;
class SegmentIndexMeta;
class TableIndexMeta;
class DBMeta;
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
struct MetaBaseCache;
struct CacheInfo;
class NewCatalog;
class NewTxnManager;

export struct CheckpointOption {
    TxnTimeStamp checkpoint_ts_ = 0;
};

export struct ChunkInfoForCreateIndex {
    std::string db_id_{};
    std::string table_id_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
};

export class NewTxn : public std::enable_shared_from_this<NewTxn> {
public:
    // For new txn
    explicit NewTxn(NewTxnManager *txn_manager,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    TxnTimeStamp last_kv_commit_ts,
                    TxnTimeStamp last_commit_ts,
                    std::unique_ptr<KVInstance> kv_instance,
                    std::shared_ptr<std::string> txn_text,
                    TransactionType txn_type);

    // For replay txn
    explicit NewTxn(BufferManager *buffer_mgr,
                    NewTxnManager *txn_mgr,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    std::unique_ptr<KVInstance> kv_instance,
                    TransactionType txn_type);

    virtual ~NewTxn();

    static std::unique_ptr<NewTxn> NewReplayTxn(NewTxnManager *txn_mgr,
                                                TransactionID txn_id,
                                                TxnTimeStamp begin_ts,
                                                TxnTimeStamp commit_ts,
                                                std::unique_ptr<KVInstance> kv_instance);

    static std::unique_ptr<NewTxn> NewRecoveryTxn(NewTxnManager *txn_mgr, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

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

    bool CheckConflictTxnStores(std::shared_ptr<NewTxn> check_txn, std::string &conflict_reason, bool &retry_query);

    Status PrepareCommit();

    void CommitBottom();

    void NotifyTopHalf();

    void PostCommit();

    void CancelCommitBottom();

    Status PostRollback(TxnTimeStamp abort_ts);

    Status Rollback();

    // Database OPs
    Status CreateDatabase(const std::string &db_name, ConflictType conflict_type, const std::shared_ptr<std::string> &comment);

    Status DropDatabase(const std::string &db_name, ConflictType conflict_type);

    // bool CheckDatabaseExists(const std::string &db_name);

    std::tuple<std::shared_ptr<DatabaseInfo>, Status> GetDatabaseInfo(const std::string &db_name);

    Status ListDatabase(std::vector<std::string> &db_names);

    // Table and Collection OPs
    Status GetTables(const std::string &db_name, std::vector<std::shared_ptr<TableDetail>> &output_table_array);

    Status CreateTable(const std::string &db_name, const std::shared_ptr<TableDef> &table_def, ConflictType conflict_type);

    Status AddColumns(const std::string &db_name, const std::string &table_name, const std::vector<std::shared_ptr<ColumnDef>> &column_defs);

    Status DropColumns(const std::string &db_name, const std::string &table_name, const std::vector<std::string> &column_names);

    Status DropTable(const std::string &db_name, const std::string &table_name, ConflictType conflict_type);

    Status RenameTable(const std::string &db_name, const std::string &old_table_name, const std::string &new_table_name);

    Status ListTable(const std::string &db_name, std::vector<std::string> &table_names);

    std::tuple<std::shared_ptr<TableInfo>, Status> GetTableInfo(const std::string &db_name, const std::string &table_name);

    std::tuple<std::shared_ptr<TableIndexInfo>, Status>
    GetTableIndexInfo(const std::string &db_name, const std::string &table_name, const std::string &index_name);

    std::tuple<std::shared_ptr<SegmentIndexInfo>, Status>
    GetSegmentIndexInfo(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id);

    std::tuple<std::vector<std::pair<ChunkID, std::shared_ptr<ChunkIndexMetaInfo>>>, Status>
    GetChunkIndexesInfo(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id);

    std::tuple<std::shared_ptr<ChunkIndexMetaInfo>, Status> GetChunkIndexInfo(const std::string &db_name,
                                                                              const std::string &table_name,
                                                                              const std::string &index_name,
                                                                              SegmentID segment_id,
                                                                              ChunkID chunk_id);

    std::tuple<std::shared_ptr<SegmentInfo>, Status> GetSegmentInfo(const std::string &db_name, const std::string &table_name, SegmentID segment_id);

    std::tuple<std::vector<std::shared_ptr<SegmentInfo>>, Status> GetSegmentsInfo(const std::string &db_name, const std::string &table_name);

    std::tuple<std::shared_ptr<BlockInfo>, Status>
    GetBlockInfo(const std::string &db_name, const std::string &table_name, SegmentID segment_id, BlockID block_id);

    std::tuple<std::vector<std::shared_ptr<BlockInfo>>, Status>
    GetBlocksInfo(const std::string &db_name, const std::string &table_name, SegmentID segment_id);

    std::tuple<std::shared_ptr<BlockColumnInfo>, Status>
    GetBlockColumnInfo(const std::string &db_name, const std::string &table_name, SegmentID segment_id, BlockID block_id, ColumnID column_id);

    // std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> GetTableSnapshot(const std::string &db_name, const std::string &table_name);

    // Status ApplyTableSnapshot(const std::shared_ptr<TableSnapshotInfo> &table_snapshot_info);

    // Index OPs

    Status
    CreateIndex(const std::string &db_name, const std::string &table_name, const std::shared_ptr<IndexBase> &index_base, ConflictType conflict_type);

    Status DropIndexByName(const std::string &db_name, const std::string &table_name, const std::string &index_name, ConflictType conflict_type);

    Status DumpMemIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name);

    Status DumpMemIndex(const std::string &db_name,
                        const std::string &table_name,
                        const std::string &index_name,
                        SegmentID segment_id,
                        RowID begin_row_id = RowID());

    Status OptimizeAllIndexes();

    Status OptimizeTableIndexes(const std::string &db_name, const std::string &table_name);

    Status OptimizeIndex(const std::string &db_name, const std::string &table_name, const std::string &index_name, SegmentID segment_id);

    // // Snapshot OPs
    Status CreateTableSnapshot(const std::string &db_name, const std::string &table_name, const std::string &snapshot_name);

    // std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> GetTableSnapshotInfo(const std::string &db_name, const std::string &table_name);

    Status RestoreTableSnapshot(const std::string &db_name, const std::shared_ptr<TableSnapshotInfo> &table_snapshot_info);

    Status RestoreTableIndexesFromSnapshot(TableMeta &table_meta, const std::vector<WalCmdCreateIndexV2> &index_cmds, bool is_link_files = false);

    std::tuple<std::shared_ptr<DatabaseSnapshotInfo>, Status> GetDatabaseSnapshotInfo(const std::string &db_name);

    Status RestoreDatabaseSnapshot(const std::shared_ptr<DatabaseSnapshotInfo> &database_snapshot_info);

    friend class NewTxnManager;

private:
    Status OptimizeIndexInner(SegmentIndexMeta &segment_index_meta,
                              const std::string &index_name,
                              const std::string &table_name,
                              const std::string &db_name,
                              const std::string &table_key);

public:
    Status OptimizeIndexByParams(const std::string &db_name,
                                 const std::string &table_name,
                                 const std::string &index_name,
                                 std::vector<std::unique_ptr<InitParameter>> params);

    Status ListIndex(const std::string &db_name, const std::string &table_name, std::vector<std::string> &index_names);

    Status Import(const std::string &db_name, const std::string &table_name, const std::vector<std::shared_ptr<DataBlock>> &input_blocks);

    Status Import(const std::string &db_name, const std::string &table_name, const std::vector<size_t> &block_row_cnts);

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
    Status Append(const std::string &db_name, const std::string &table_name, const std::shared_ptr<DataBlock> &input_block);

    Status Append(const TableInfo &table_info, const std::shared_ptr<DataBlock> &input_block);

    Status Update(const std::string &db_name,
                  const std::string &table_name,
                  const std::shared_ptr<DataBlock> &input_block,
                  const std::vector<RowID> &row_ids);

private:
    Status AppendInner(const std::string &db_name,
                       const std::string &table_name,
                       const std::string &table_key,
                       TableMeta &table_meta,
                       const std::shared_ptr<DataBlock> &input_block);

    Status DeleteInner(const std::string &db_name, const std::string &table_name, TableMeta &table_meta, const std::vector<RowID> &row_ids);

public:
    Status Delete(const std::string &db_name, const std::string &table_name, const std::vector<RowID> &row_ids);

    Status Compact(const std::string &db_name, const std::string &table_name, const std::vector<SegmentID> &segment_ids);

    Status CheckTableIfDelete(TableMeta &table_meta, bool &has_delete);

    Status Cleanup();

private:
    Status ReplayCompact(WalCmdCompactV2 *compact_cmd);

    Status CleanupInner(const std::vector<std::shared_ptr<MetaKey>> &metas);

public:
    Status Checkpoint(TxnTimeStamp last_ckp_ts, bool auto_checkpoint);

    // Getter
    [[nodiscard]] BufferManager *buffer_mgr() const { return buffer_mgr_; }

    [[nodiscard]] TransactionID TxnID() const;

    [[nodiscard]] TxnTimeStamp BeginTS() const;

    [[nodiscard]] TxnTimeStamp CommitTS() const;

    [[nodiscard]] TxnTimeStamp KVCommitTS() const;

    [[nodiscard]] TxnTimeStamp LastSystemKVCommitTS() const;

    [[nodiscard]] TxnTimeStamp LastSystemCommitTS() const;

    void SetTxnKVCommitTS(TxnTimeStamp kv_commit_ts);

    [[nodiscard]] TxnState GetTxnState() const;

    [[nodiscard]] TransactionType GetTxnType() const;

    bool readonly() const;

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

    NewTxnManager *txn_mgr() const { return txn_mgr_; }

    WalEntry *GetWALEntry() const;

    [[nodiscard]] std::shared_ptr<std::string> GetTxnText() const { return txn_text_; }

    [[nodiscard]] const std::string &db_name() const { return db_name_; }

    void SetDBName(const std::string &db_name) { db_name_ = db_name; }

    // Some transaction need to pass the txn access right check in txn commit phase;
    void SetReaderAllowed(bool allowed) { allowed_in_reader_ = allowed; }

    bool IsReaderAllowed() const { return allowed_in_reader_; }

    BaseTxnStore *GetTxnStore() const { return base_txn_store_.get(); }

    std::shared_ptr<TxnContext> txn_context() const { return txn_context_ptr_; }
    void AddOperation(const std::shared_ptr<std::string> &operation_text);
    std::vector<std::shared_ptr<std::string>> GetOperations() const;

    KVInstance *kv_instance() const { return kv_instance_.get(); }

    void AddMetaKeyForCommit(const std::string &key);

    TxnTimeStamp GetCurrentCkpTS() const;

private:
    void CheckTxnStatus();

    void CheckTxn(const std::string &db_name);

public:
    Status GetDBMeta(const std::string &db_name, std::shared_ptr<DBMeta> &db_meta, TxnTimeStamp &db_create_ts, std::string *db_key = nullptr);

    Status GetTableMeta(const std::string &db_name,
                        const std::string &table_name,
                        std::shared_ptr<DBMeta> &db_meta,
                        std::shared_ptr<TableMeta> &table_meta,
                        TxnTimeStamp &create_timestamp,
                        std::string *table_key = nullptr);

    Status GetTableMeta(const std::string &table_name,
                        std::shared_ptr<DBMeta> &db_meta,
                        std::shared_ptr<TableMeta> &table_meta,
                        TxnTimeStamp &create_timestamp,
                        std::string *table_key = nullptr);

    Status GetTableIndexMeta(const std::string &db_name,
                             const std::string &table_name,
                             const std::string &index_name,
                             std::shared_ptr<DBMeta> &db_meta,
                             std::shared_ptr<TableMeta> &table_meta,
                             std::shared_ptr<TableIndexMeta> &table_index_meta,
                             std::string *table_key,
                             std::string *index_key);

    Status GetTableIndexMeta(const std::string &index_name,
                             TableMeta &table_meta,
                             std::shared_ptr<TableIndexMeta> &table_index_meta,
                             std::string *index_key = nullptr);

private:
    friend struct NewTxnCompactState;

    // DML
    Status AppendInBlock(BlockMeta &block_meta, size_t block_offset, size_t append_rows, const DataBlock *input_block, size_t input_offset);

    Status AppendInColumn(ColumnMeta &column_meta, size_t dest_offset, size_t append_rows, const ColumnVector &column_vector, size_t source_offset);

    Status DeleteInBlock(BlockMeta &block_meta, const std::vector<BlockOffset> &block_offsets, std::vector<BlockOffset> &undo_block_offsets);

    Status RollbackDeleteInBlock(BlockMeta &block_meta, const std::vector<BlockOffset> &block_offsets);

    Status PrintVersionInBlock(BlockMeta &block_meta, const std::vector<BlockOffset> &block_offsets, bool ignore_invisible);

    Status CompactBlock(BlockMeta &block_meta, NewTxnCompactState &compact_state);

    Status AddColumnsData(TableMeta &table_meta, const std::vector<std::shared_ptr<ColumnDef>> &column_defs, const std::vector<u32> &column_idx_list);

    Status AddColumnsDataInSegment(SegmentMeta &segment_meta,
                                   const std::vector<std::shared_ptr<ColumnDef>> &column_defs,
                                   const std::vector<u32> &column_idx_list,
                                   const std::vector<Value> &default_values);

    Status AddColumnsDataInBlock(BlockMeta &block_meta,
                                 const std::vector<std::shared_ptr<ColumnDef>> &column_defs,
                                 const std::vector<u32> &column_idx_list,
                                 const std::vector<Value> &default_values);

    Status DropColumnsData(TableMeta &table_meta, const std::vector<ColumnID> &column_ids);

    Status AppendIndex(TableIndexMeta &table_index_meta, const std::pair<RowID, u64> &append_range);

    Status AppendMemIndex(SegmentIndexMeta &segment_index_meta, BlockID block_id, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt);

    Status PopulateIndex(const std::string &db_name,
                         const std::string &table_name,
                         const std::string &index_name,
                         const std::string &table_key,
                         TableIndexMeta &table_index_meta,
                         SegmentMeta &segment_meta,
                         size_t segment_row_cnt,
                         DumpIndexCause dump_index_cause,
                         WalCmdCreateIndexV2 *create_index_cmd_ptr = nullptr);

    Status ReplayDumpIndex(WalCmdDumpIndexV2 *dump_index_cmd);

    Status InitSegmentIndex(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta);

    Status PopulateIndexToMem(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, ColumnID column_id, size_t segment_row_cnt);

    Status PopulateFtIndexInner(std::shared_ptr<IndexBase> index_base,
                                SegmentIndexMeta &segment_index_meta,
                                SegmentMeta &segment_meta,
                                ColumnID column_id,
                                size_t segment_row_cnt,
                                std::vector<ChunkID> &new_chunk_ids);

    Status PopulateIvfIndexInner(std::shared_ptr<IndexBase> index_base,
                                 SegmentIndexMeta &segment_index_meta,
                                 SegmentMeta &segment_meta,
                                 std::shared_ptr<ColumnDef> column_def,
                                 std::vector<ChunkID> &new_chunk_ids);

    Status PopulateEmvbIndexInner(std::shared_ptr<IndexBase> index_base,
                                  SegmentIndexMeta &segment_index_meta,
                                  SegmentMeta &segment_meta,
                                  std::shared_ptr<ColumnDef> column_def,
                                  std::vector<ChunkID> &new_chunk_ids);

    Status OptimizeFtIndex(std::shared_ptr<IndexBase> index_base,
                           SegmentIndexMeta &segment_index_meta,
                           RowID &base_rowid,
                           u32 &row_cnt,
                           u32 &term_cnt,
                           std::string &base_name);

    Status OptimizeVecIndex(std::shared_ptr<IndexBase> index_base,
                            std::shared_ptr<ColumnDef> column_def,
                            SegmentMeta &segment_meta,
                            RowID base_rowid,
                            u32 row_cnt,
                            BufferObj *buffer_obj);

    Status OptimizeSegmentIndexByParams(SegmentIndexMeta &segment_index_meta, const std::vector<std::unique_ptr<InitParameter>> &params);

    Status ReplayOptimizeIndeByParams(WalCmdOptimizeV2 *optimize_cmd);

    Status DumpSegmentMemIndex(SegmentIndexMeta &segment_index_meta, const ChunkID &new_chunk_id);

    Status CheckpointDB(DBMeta &db_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store);

    Status CheckpointTable(TableMeta &table_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store);

    Status
    CountMemIndexGapInSegment(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, std::vector<std::pair<RowID, u64>> &append_ranges);

public:
    Status RecoverMemIndex(TableIndexMeta &table_index_meta);

    Status CommitMemIndex(TableIndexMeta &table_index_meta);

    Status GetFullTextIndexReader(const std::string &db_name, const std::string &table_name, std::shared_ptr<IndexReader> &index_reader);

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
    Status PrepareCommitReplayImport(WalCmdImportV2 *import_cmd);
    Status CommitBottomAppend(WalCmdAppendV2 *append_cmd);
    Status PrepareCommitDelete(const WalCmdDeleteV2 *delete_cmd);
    Status RollbackDelete(const DeleteTxnStore *delete_txn_store);
    Status PrepareCommitCompact(WalCmdCompactV2 *compact_cmd);
    Status PrepareCommitDumpIndex(const WalCmdDumpIndexV2 *dump_index_cmd, KVInstance *kv_instance);
    Status PrepareCommitCheckpoint(const WalCmdCheckpointV2 *checkpoint_cmd);
    Status CommitCheckpointDB(DBMeta &db_meta, const WalCmdCheckpointV2 *checkpoint_cmd);
    Status CommitCheckpointTable(TableMeta &table_meta, const WalCmdCheckpointV2 *checkpoint_cmd);
    Status CommitCheckpointTableData(TableMeta &table_meta, TxnTimeStamp checkpoint_ts);
    Status PrepareCommitCreateTableSnapshot(const WalCmdCreateTableSnapshot *create_table_snapshot_cmd);
    Status PrepareCommitRestoreTableSnapshot(const WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, bool is_link_files = false);
    Status PrepareCommitRestoreDatabaseSnapshot(const WalCmdRestoreDatabaseSnapshot *restore_database_snapshot_cmd);
    Status CommitBottomCreateTableSnapshot(WalCmdCreateTableSnapshot *create_table_snapshot_cmd);
    Status CheckpointForSnapshot(TxnTimeStamp last_ckp_ts, CheckpointTxnStore *txn_store);

    Status AddSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta);
    Status CommitSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta);
    Status FlushVersionFile(BlockMeta &block_meta, TxnTimeStamp save_ts);
    Status FlushColumnFiles(BlockMeta &block_meta, TxnTimeStamp save_ts);

    Status IncrLatestID(std::string &id_str, std::string_view id_name) const;

    // Check transaction conflicts
    bool CheckConflictTxnStore(NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateDBTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropDBTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateTableTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const AppendTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const ImportTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const AddColumnsTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropColumnsTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CompactTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateIndexTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropIndexTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const OptimizeIndexTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DumpMemIndexTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DeleteTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const DropTableTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const RenameTableTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const UpdateTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const RestoreTableTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const RestoreDatabaseTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);
    bool CheckConflictTxnStore(const CreateTableSnapshotTxnStore &txn_store, NewTxn *previous_txn, std::string &cause, bool &retry_query);

public:
    bool IsReplay() const;

    Status ReplayWalCmd(const std::shared_ptr<WalCmd> &wal_cmd, TxnTimeStamp commit_ts, i64 txn_id);

    Status GetDBFilePaths(const std::string &db_name, std::vector<std::string> &file_paths);
    Status GetTableFilePaths(const std::string &db_name, const std::string &table_name, std::vector<std::string> &file_paths);
    Status GetSegmentFilePaths(const std::string &db_name, const std::string &table_name, SegmentID segment_id, std::vector<std::string> &file_paths);
    Status GetBlockFilePaths(const std::string &db_name,
                             const std::string &table_name,
                             SegmentID segment_id,
                             BlockID block_id,
                             std::vector<std::string> &file_paths);
    Status GetBlockColumnFilePaths(const std::string &db_name,
                                   const std::string &table_name,
                                   SegmentID segment_id,
                                   BlockID block_id,
                                   ColumnID column_id,
                                   std::vector<std::string> &file_paths);
    Status GetColumnFilePaths(const std::string &db_name,
                              const std::string &table_name,
                              const std::string &column_name,
                              std::vector<std::string> &file_paths);
    Status GetTableIndexFilePaths(const std::string &db_name,
                                  const std::string &table_name,
                                  const std::string &index_name,
                                  std::vector<std::string> &file_paths);
    Status GetSegmentIndexFilepaths(const std::string &db_name,
                                    const std::string &table_name,
                                    const std::string &index_name,
                                    SegmentID segment_id,
                                    std::vector<std::string> &file_paths);
    Status GetChunkIndexFilePaths(const std::string &db_name,
                                  const std::string &table_name,
                                  const std::string &index_name,
                                  SegmentID segment_id,
                                  ChunkID chunk_id,
                                  std::vector<std::string> &file_paths);
    Status ProcessSnapshotRestorationData(const std::string &db_name,
                                          const std::string &db_id_str,
                                          const std::string &table_name,
                                          const std::string &table_id_str,
                                          const std::shared_ptr<TableDef> &table_def,
                                          const std::shared_ptr<TableSnapshotInfo> &table_snapshot_info,
                                          const std::string &snapshot_name,
                                          RestoreTableTxnStore *txn_store);
    Status RestoreTableFromSnapshot(const WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, DBMeta &db_meta, bool is_link_files = false);
    Status ManualDumpIndex(const std::string &db_name, const std::string &table_name);

    Status Dummy();
    void SetWalSize(i64 wal_size);

    // Get the table id which is used in the txn. Return empty string if no table is used.
    std::string GetTableIdStr();

    void AddSemaphore(std::unique_ptr<std::binary_semaphore> sema);
    const std::vector<std::unique_ptr<std::binary_semaphore>> &semas() const;

    void AddMetaCache(const std::shared_ptr<MetaBaseCache> &meta_base_cache);
    void AddCacheInfo(const std::shared_ptr<CacheInfo> &cache_info);
    void ResetMetaCacheAndCacheInfo();
    void SaveMetaCacheAndCacheInfo();

    Status WriteDataBlockToFile(const std::string &db_name,
                                const std::string &table_name,
                                std::shared_ptr<DataBlock> input_block,
                                const u64 &input_block_idx,
                                std::vector<std::string> *object_paths = nullptr);

private:
    // Reference to external class
    NewTxnManager *txn_mgr_{};
    BufferManager *buffer_mgr_{}; // This BufferManager ptr Only for replaying wal
    NewCatalog *new_catalog_{};

    // Used to store the local data in this transaction
    std::shared_ptr<BaseTxnStore> base_txn_store_{};

    NewTxnStore txn_store_; // this has this ptr, so txn cannot be moved.

    // Use as txn context;
    mutable std::shared_mutex rw_locker_{};

    // Handled database
    std::string db_name_{};

    /// LOG
    // WalEntry
    std::shared_ptr<WalEntry> wal_entry_{};

    // WalManager notify the commit bottom half is done
    std::mutex commit_lock_{};
    std::condition_variable commit_cv_{};
    bool commit_bottom_done_{false};

    // KV txn instance
    std::unique_ptr<KVInstance> kv_instance_{};

    // std::string
    std::shared_ptr<std::string> txn_text_{nullptr};

    // ADMIN command which allowed in follower and learner
    bool allowed_in_reader_{false};

    std::vector<std::string> keys_wait_for_commit_{};

    // Used for new checkpoint
    TxnTimeStamp current_ckp_ts_{};
    size_t wal_size_{};

    // Use for commit and rollback
    std::vector<std::unique_ptr<std::binary_semaphore>> semas_{};

    // Meta cache
    std::vector<std::shared_ptr<MetaBaseCache>> meta_cache_items_{}; // cache item to store

    std::vector<std::shared_ptr<CacheInfo>> cache_infos_{};

private:
    std::shared_ptr<TxnContext> txn_context_ptr_{};

    friend class NewTxnTableStore;

private:
    void WaitForCompletion();
    void SetCompletion();

public:
    std::shared_ptr<NewTxn> conflicted_txn_{};
    std::mutex finished_mutex_{};
    std::condition_variable finished_cv_{};
    bool finished_{false};

private:
    std::vector<ChunkInfoForCreateIndex> chunk_infos_; // For cleanup when create index is rollbacked

    bool bottom_done_{false}; // TODO: Use a transaction state instead of a bool flag
};

} // namespace infinity