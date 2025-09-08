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

export module infinity_core:base_txn_store;

import :txn_state;
import :wal_entry;

import std;

import column_def;
import internal_types;

namespace infinity {

struct DataBlock;
class IndexBase;
class TableDef;
struct EraseBaseCache;

export struct MemIndexRange {
    std::string index_id_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    SegmentOffset start_offset_{};
    size_t row_count_{};
};

// enum class TxnStoreType {
//     kInvalid,
//     kCreateDB,
//     kCreateTable,
//     kAppend,
//     kImport,
//     kDelete,
//     kUpdate,
//     kCreateIndex,
//     kDumpMemIndex,
//     kOptimizeIndex,
//     kCompact,
// };
// kCheckpoint, // Develop know it's a checkpoint txn
// kRead,       // Developer know it's a read txn
// kNormal,     // Developer doesn't know what type is this txn
// kReplay,
// kRecovery,
// kNewCheckpoint,
// kAppend,
// kUpdate,
// kImport,
// kDumpMemIndex,
// kOptimizeIndex,
// kCreateIndex,
// kDelete,
// kCompact,

export struct BaseTxnStore {
    explicit BaseTxnStore(TransactionType type) : type_(type) {};

    TransactionType type_{TransactionType::kInvalid};

    virtual std::string ToString() const = 0;
    virtual std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const = 0;
    virtual std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const = 0;
    virtual ~BaseTxnStore() = default;
    virtual void ClearData();
};

// DummyTxnStore is only used in test
export struct DummyTxnStore final : public BaseTxnStore {
    DummyTxnStore() : BaseTxnStore(TransactionType::kNormal) {}
    ~DummyTxnStore() override = default;

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct CreateDBTxnStore final : public BaseTxnStore {
    CreateDBTxnStore() : BaseTxnStore(TransactionType::kCreateDB) {}
    ~CreateDBTxnStore() override = default;

    std::string db_name_{};
    u64 db_id_{};
    std::string db_id_str_{};
    std::shared_ptr<std::string> comment_ptr_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct DropDBTxnStore final : public BaseTxnStore {
    DropDBTxnStore() : BaseTxnStore(TransactionType::kDropDB) {}
    ~DropDBTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    TxnTimeStamp create_ts_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct CreateTableTxnStore final : public BaseTxnStore {
    CreateTableTxnStore() : BaseTxnStore(TransactionType::kCreateTable) {}
    ~CreateTableTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 table_id_{};
    std::shared_ptr<TableDef> table_def_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct CreateTableSnapshotTxnStore final : public BaseTxnStore {
    CreateTableSnapshotTxnStore() : BaseTxnStore(TransactionType::kCreateTableSnapshot) {}

    std::string db_name_{};
    std::string table_name_{};
    std::string snapshot_name_{};
    TxnTimeStamp max_commit_ts_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct RestoreTableTxnStore final : public BaseTxnStore {
    RestoreTableTxnStore() : BaseTxnStore(TransactionType::kRestoreTable) {}

    std::string db_name_{};
    std::string snapshot_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 table_id_{};

    std::shared_ptr<TableDef> table_def_{};
    std::vector<WalSegmentInfoV2> segment_infos_{};
    std::vector<WalCmdCreateIndexV2> index_cmds_{};
    std::vector<std::string> files_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct RestoreDatabaseTxnStore final : public BaseTxnStore {
    RestoreDatabaseTxnStore() : BaseTxnStore(TransactionType::kRestoreDatabase) {}

    std::string db_name_{};
    std::string db_id_str_{};
    std::string db_comment_{};
    std::vector<std::shared_ptr<RestoreTableTxnStore>> restore_table_txn_stores_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct DropTableTxnStore final : public BaseTxnStore {
    DropTableTxnStore() : BaseTxnStore(TransactionType::kDropTable) {}
    ~DropTableTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 table_id_{};
    TxnTimeStamp create_ts_{};
    std::string table_key_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct RenameTableTxnStore final : public BaseTxnStore {
    RenameTableTxnStore() : BaseTxnStore(TransactionType::kRenameTable) {}
    ~RenameTableTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string old_table_name_{};
    std::string table_id_str_{};
    std::string new_table_name_{};
    std::string old_table_key_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct CreateIndexTxnStore final : public BaseTxnStore {
    CreateIndexTxnStore() : BaseTxnStore(TransactionType::kCreateIndex) {}
    ~CreateIndexTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 table_id_{};
    std::shared_ptr<IndexBase> index_base_{};
    std::string index_id_str_{};
    std::string table_key_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct DropIndexTxnStore final : public BaseTxnStore {
    DropIndexTxnStore() : BaseTxnStore(TransactionType::kDropIndex) {}
    ~DropIndexTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 table_id_{};
    std::string index_name_{};
    std::string index_id_str_{};
    u64 index_id_{};
    TxnTimeStamp create_ts_{};
    std::string index_key_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct OptimizeIndexStoreEntry {
    std::string db_name_{};
    std::string db_id_str_{};
    u64 db_id_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 table_id_{};
    std::string table_key_{};
    std::string index_name_{};
    std::string index_id_str_{};
    u64 index_id_{};
    SegmentID segment_id_{};
    std::vector<WalChunkIndexInfo> new_chunk_infos_;
    std::vector<ChunkID> deprecate_chunks_;
};

export struct OptimizeIndexTxnStore final : public BaseTxnStore {
    OptimizeIndexTxnStore() : BaseTxnStore(TransactionType::kOptimizeIndex) {}
    ~OptimizeIndexTxnStore() override = default;

    std::vector<std::string> db_names_{};
    std::map<std::string, std::vector<std::string>> table_names_in_db_{};
    std::vector<OptimizeIndexStoreEntry> entries_;

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct AppendTxnStore final : public BaseTxnStore {
    AppendTxnStore() : BaseTxnStore(TransactionType::kAppend) {}
    ~AppendTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    std::shared_ptr<DataBlock> input_block_{};
    std::vector<std::string> index_ids_{}; // indexes will be appended

    // For data append
    std::vector<std::pair<RowID, u64>> row_ranges_{};

    // For mem index
    std::vector<MemIndexRange> mem_indexes_to_append_{};
    std::vector<MemIndexRange> mem_indexes_to_dump_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    void ClearData() final;
    size_t RowCount() const;
};

export struct ImportTxnStore final : public BaseTxnStore {
    ImportTxnStore() : BaseTxnStore(TransactionType::kImport) {}
    ~ImportTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};
    std::string table_key_{};

    std::map<SegmentID, std::vector<std::shared_ptr<DataBlock>>> input_blocks_in_imports_{};
    std::vector<WalSegmentInfo> segment_infos_{};

    std::vector<std::string> index_names_{};
    std::vector<std::string> index_ids_str_{};
    std::vector<u64> index_ids_{};
    std::vector<SegmentID> segment_ids_{};
    std::map<SegmentID, std::vector<WalChunkIndexInfo>> chunk_infos_in_segments_{};
    std::map<SegmentID, std::vector<ChunkID>> deprecate_ids_in_segments_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    void ClearData() final;
    size_t RowCount() const;
    size_t SegmentCount() const;
};

export struct DumpMemIndexTxnStore final : public BaseTxnStore {
    DumpMemIndexTxnStore() : BaseTxnStore(TransactionType::kDumpMemIndex) {}
    ~DumpMemIndexTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    std::string index_name_{};
    std::string index_id_str_{};
    u64 index_id_{};
    std::vector<SegmentID> segment_ids_{};
    std::map<SegmentID, std::vector<WalChunkIndexInfo>> chunk_infos_in_segments_{};
    std::string table_key_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct AddColumnsTxnStore final : public BaseTxnStore {
    AddColumnsTxnStore() : BaseTxnStore(TransactionType::kAddColumn) {}
    ~AddColumnsTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    std::vector<u32> column_idx_list_{};
    std::vector<std::shared_ptr<ColumnDef>> column_defs_{};
    std::string table_key_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct DropColumnsTxnStore final : public BaseTxnStore {
    DropColumnsTxnStore() : BaseTxnStore(TransactionType::kDropColumn) {}
    ~DropColumnsTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    std::vector<std::string> column_names_{};
    std::vector<ColumnID> column_ids_{};
    std::string table_key_{};
    std::vector<std::string> column_keys_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct CompactTxnStore final : public BaseTxnStore {
    CompactTxnStore() : BaseTxnStore(TransactionType::kCompact) {}
    ~CompactTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};
    std::string table_key_{};

    SegmentID new_segment_id_{};
    std::vector<WalSegmentInfo> segment_infos_{};
    std::vector<SegmentID> deprecated_segment_ids_{};

    std::vector<std::string> index_names_{};
    std::vector<std::string> index_ids_str_{};
    std::vector<u64> index_ids_{};
    std::vector<SegmentID> segment_ids_{};
    std::map<SegmentID, std::vector<WalChunkIndexInfo>> chunk_infos_in_segments_{};
    std::map<SegmentID, std::vector<ChunkID>> deprecate_ids_in_segments_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct DeleteTxnStore final : public BaseTxnStore {
    DeleteTxnStore() : BaseTxnStore(TransactionType::kDelete) {}
    ~DeleteTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    std::vector<RowID> row_ids_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct UpdateTxnStore final : public BaseTxnStore {
    UpdateTxnStore() : BaseTxnStore(TransactionType::kUpdate) {}
    ~UpdateTxnStore() override = default;

    std::string db_name_{};
    std::string db_id_str_{};
    std::string table_name_{};
    std::string table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    std::vector<std::shared_ptr<DataBlock>> input_blocks_{};
    std::vector<std::string> index_ids_{}; // indexes will be appended

    // For data append
    std::vector<std::pair<RowID, u64>> row_ranges_{};

    // For mem index
    std::vector<MemIndexRange> mem_indexes_to_append_{};
    std::vector<MemIndexRange> mem_indexes_to_dump_{};

    // For delete
    std::vector<RowID> row_ids_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;

    void ClearData() final;
    size_t RowCount() const;
};

export struct FlushDataEntry {
    explicit FlushDataEntry(const std::string &db_id_str, const std::string &table_id_str, SegmentID segment_id, BlockID block_id)
        : db_id_str_(db_id_str), table_id_str_(table_id_str), segment_id_(segment_id), block_id_(block_id) {}
    std::string db_id_str_{};
    std::string table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    std::string to_flush_{};
};
export struct CheckpointTxnStore final : public BaseTxnStore {
    explicit CheckpointTxnStore(TxnTimeStamp checkpoint_ts, bool auto_checkpoint)
        : BaseTxnStore(TransactionType::kNewCheckpoint), max_commit_ts_(checkpoint_ts), auto_check_point_(auto_checkpoint) {}
    ~CheckpointTxnStore() override = default;

    std::vector<std::shared_ptr<FlushDataEntry>> entries_{};
    i64 max_commit_ts_{};
    bool auto_check_point_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

export struct CleanupTxnStore final : public BaseTxnStore {
    CleanupTxnStore() : BaseTxnStore(TransactionType::kCleanup) {}
    ~CleanupTxnStore() override = default;

    i64 timestamp_{};

    std::string ToString() const final;
    std::shared_ptr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    std::vector<std::shared_ptr<EraseBaseCache>> ToCachedMeta(TxnTimeStamp commit_ts) const final;
};

} // namespace infinity