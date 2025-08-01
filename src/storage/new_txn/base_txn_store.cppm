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

module;

#include <vector>
#include <ranges>

export module infinity_core:base_txn_store;

import :stl;
import internal_types;
import :txn_state;
import column_def;
import :wal_entry;

namespace infinity {

struct DataBlock;
class IndexBase;
class TableDef;

export struct MemIndexRange {
    String index_id_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    SegmentOffset start_offset_{};
    SizeT row_count_{};
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

    virtual String ToString() const = 0;
    virtual SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const = 0;
    virtual ~BaseTxnStore() = default;
    virtual void ClearData();
};

// DummyTxnStore is only used in test
export struct DummyTxnStore final : public BaseTxnStore {
    DummyTxnStore() : BaseTxnStore(TransactionType::kNormal) {}
    ~DummyTxnStore() override = default;

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct CreateDBTxnStore final : public BaseTxnStore {
    CreateDBTxnStore() : BaseTxnStore(TransactionType::kCreateDB) {}
    ~CreateDBTxnStore() override = default;

    String db_name_{};
    u64 db_id_{};
    String db_id_str_{};
    SharedPtr<String> comment_ptr_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct DropDBTxnStore final : public BaseTxnStore {
    DropDBTxnStore() : BaseTxnStore(TransactionType::kDropDB) {}
    ~DropDBTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    TxnTimeStamp create_ts_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct CreateTableTxnStore final : public BaseTxnStore {
    CreateTableTxnStore() : BaseTxnStore(TransactionType::kCreateTable) {}
    ~CreateTableTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    String table_name_{};
    String table_id_str_{};
    u64 table_id_{};
    SharedPtr<TableDef> table_def_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct DropTableTxnStore final : public BaseTxnStore {
    DropTableTxnStore() : BaseTxnStore(TransactionType::kDropTable) {}
    ~DropTableTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    String table_name_{};
    String table_id_str_{};
    u64 table_id_{};
    TxnTimeStamp create_ts_{};
    String table_key_{};

    String ToString() const final;\
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct RenameTableTxnStore final : public BaseTxnStore {
    RenameTableTxnStore() : BaseTxnStore(TransactionType::kRenameTable) {}
    ~RenameTableTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String old_table_name_{};
    String table_id_str_{};
    String new_table_name_{};
    String old_table_key_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct CreateIndexTxnStore final : public BaseTxnStore {
    CreateIndexTxnStore() : BaseTxnStore(TransactionType::kCreateIndex) {}
    ~CreateIndexTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    String table_name_{};
    String table_id_str_{};
    u64 table_id_{};
    SharedPtr<IndexBase> index_base_{};
    String index_id_str_{};
    String table_key_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct DropIndexTxnStore final : public BaseTxnStore {
    DropIndexTxnStore() : BaseTxnStore(TransactionType::kDropIndex) {}
    ~DropIndexTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    String table_name_{};
    String table_id_str_{};
    u64 table_id_{};
    String index_name_{};
    String index_id_str_{};
    u64 index_id_{};
    TxnTimeStamp create_ts_{};
    String index_key_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct OptimizeIndexStoreEntry {
    String db_name_{};
    String db_id_str_{};
    u64 db_id_{};
    String table_name_{};
    String table_id_str_{};
    u64 table_id_{};
    String table_key_{};
    String index_name_{};
    String index_id_str_{};
    u64 index_id_{};
    SegmentID segment_id_{};
    std::vector<WalChunkIndexInfo> new_chunk_infos_;
    Vector<ChunkID> deprecate_chunks_;
};

export struct OptimizeIndexTxnStore final : public BaseTxnStore {
    OptimizeIndexTxnStore() : BaseTxnStore(TransactionType::kOptimizeIndex) {}
    ~OptimizeIndexTxnStore() override = default;

    Vector<String> db_names_{};
    Map<String, Vector<String>> table_names_in_db_{};
    Vector<OptimizeIndexStoreEntry> entries_;

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct AppendTxnStore final : public BaseTxnStore {
    AppendTxnStore() : BaseTxnStore(TransactionType::kAppend) {}
    ~AppendTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    SharedPtr<DataBlock> input_block_{};
    Vector<String> index_ids_{}; // indexes will be appended

    // For data append
    Vector<Pair<RowID, u64>> row_ranges_{};

    // For mem index
    Vector<MemIndexRange> mem_indexes_to_append_{};
    Vector<MemIndexRange> mem_indexes_to_dump_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    void ClearData() final;
    SizeT RowCount() const;
};

export struct ImportTxnStore final : public BaseTxnStore {
    ImportTxnStore() : BaseTxnStore(TransactionType::kImport) {}
    ~ImportTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};
    String table_key_{};

    Map<SegmentID, Vector<SharedPtr<DataBlock>>> input_blocks_in_imports_{};
    Vector<WalSegmentInfo> segment_infos_{};

    Vector<String> index_names_{};
    Vector<String> index_ids_str_{};
    Vector<u64> index_ids_{};
    Vector<SegmentID> segment_ids_{};
    Map<SegmentID, Vector<WalChunkIndexInfo>> chunk_infos_in_segments_{};
    Map<SegmentID, Vector<ChunkID>> deprecate_ids_in_segments_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    void ClearData() final;
    SizeT RowCount() const;
    SizeT SegmentCount() const;
};

export struct DumpMemIndexTxnStore final : public BaseTxnStore {
    DumpMemIndexTxnStore() : BaseTxnStore(TransactionType::kDumpMemIndex) {}
    ~DumpMemIndexTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    String index_name_{};
    String index_id_str_{};
    u64 index_id_{};
    Vector<SegmentID> segment_ids_{};
    Map<SegmentID, Vector<WalChunkIndexInfo>> chunk_infos_in_segments_{};
    String table_key_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct AddColumnsTxnStore final : public BaseTxnStore {
    AddColumnsTxnStore() : BaseTxnStore(TransactionType::kAddColumn) {}
    ~AddColumnsTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SharedPtr<ColumnDef>> column_defs_{};
    String table_key_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct DropColumnsTxnStore final : public BaseTxnStore {
    DropColumnsTxnStore() : BaseTxnStore(TransactionType::kDropColumn) {}
    ~DropColumnsTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<String> column_names_{};
    Vector<ColumnID> column_ids_{};
    String table_key_{};
    Vector<String> column_keys_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct CompactTxnStore final : public BaseTxnStore {
    CompactTxnStore() : BaseTxnStore(TransactionType::kCompact) {}
    ~CompactTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};
    String table_key_{};

    SegmentID new_segment_id_{};
    Vector<WalSegmentInfo> segment_infos_{};
    Vector<SegmentID> deprecated_segment_ids_{};

    Vector<String> index_names_{};
    Vector<String> index_ids_str_{};
    Vector<u64> index_ids_{};
    Vector<SegmentID> segment_ids_{};
    Map<SegmentID, Vector<WalChunkIndexInfo>> chunk_infos_in_segments_{};
    Map<SegmentID, Vector<ChunkID>> deprecate_ids_in_segments_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct DeleteTxnStore final : public BaseTxnStore {
    DeleteTxnStore() : BaseTxnStore(TransactionType::kDelete) {}
    ~DeleteTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<RowID> row_ids_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct UpdateTxnStore final : public BaseTxnStore {
    UpdateTxnStore() : BaseTxnStore(TransactionType::kUpdate) {}
    ~UpdateTxnStore() override = default;

    String db_name_{};
    String db_id_str_{};
    String table_name_{};
    String table_id_str_{};
    u64 db_id_{};
    u64 table_id_{};

    Vector<SharedPtr<DataBlock>> input_blocks_{};
    Vector<String> index_ids_{}; // indexes will be appended

    // For data append
    Vector<Pair<RowID, u64>> row_ranges_{};

    // For mem index
    Vector<MemIndexRange> mem_indexes_to_append_{};
    Vector<MemIndexRange> mem_indexes_to_dump_{};

    // For delete
    Vector<RowID> row_ids_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
    void ClearData() final;
    SizeT RowCount() const;
};

export struct FlushDataEntry {
    explicit FlushDataEntry(const String &db_id_str, const String &table_id_str, SegmentID segment_id, BlockID block_id)
        : db_id_str_(db_id_str), table_id_str_(table_id_str), segment_id_(segment_id), block_id_(block_id) {}
    String db_id_str_{};
    String table_id_str_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    String to_flush_{};
};
export struct CheckpointTxnStore final : public BaseTxnStore {
    CheckpointTxnStore() : BaseTxnStore(TransactionType::kNewCheckpoint) {}
    ~CheckpointTxnStore() override = default;

    Vector<SharedPtr<FlushDataEntry>> entries_{};
    i64 max_commit_ts_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

export struct CleanupTxnStore final : public BaseTxnStore {
    CleanupTxnStore() : BaseTxnStore(TransactionType::kCleanup) {}
    ~CleanupTxnStore() override = default;

    i64 timestamp_{};

    String ToString() const final;
    SharedPtr<WalEntry> ToWalEntry(TxnTimeStamp commit_ts) const final;
};

} // namespace infinity