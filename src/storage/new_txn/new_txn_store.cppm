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

export module new_txn_store;

import stl;

import data_access_state;
import status;
import internal_types;
import index_base;
import extra_ddl_info;
import wal_entry;
import column_def;
import third_party;
import logger;

namespace infinity {

class NewTxn;
struct Catalog;
struct DBEntry;
struct TableIndexEntry;
struct TableEntry;
struct SegmentEntry;
struct BlockEntry;
struct BlockColumnEntry;
class DataBlock;
struct SegmentIndexEntry;
struct ChunkIndexEntry;
class BGTaskProcessor;
class TxnManager;
enum class CompactStatementType;
class CatalogDeltaEntry;
class BufferManager;

export struct NewTxnSegmentStore {
public:
    static NewTxnSegmentStore AddSegmentStore(SegmentEntry *segment_entry);

    explicit NewTxnSegmentStore(SegmentEntry *segment_entry);

    NewTxnSegmentStore() = default;

    void AddDeltaOp(CatalogDeltaEntry *local_delta_ops, AppendState *append_state, NewTxn *txn, bool set_sealed) const;

public:
    SegmentEntry *const segment_entry_ = nullptr;
    HashMap<BlockID, BlockEntry *> block_entries_;
    Vector<BlockColumnEntry *> block_column_entries_;
};

export struct NewTxnIndexStore {
public:
    explicit NewTxnIndexStore(TableIndexEntry *table_index_entry);
    NewTxnIndexStore() = default;

    void AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnTimeStamp commit_ts) const;

    void Commit(TransactionID txn_id, TxnTimeStamp commit_ts);

    void Rollback(TxnTimeStamp abort_ts);

    void AddSegmentOptimizing(SegmentIndexEntry *segment_index_entry);

    bool TryRevert();

public:
    TableIndexEntry *const table_index_entry_{};

    HashMap<SegmentID, SegmentIndexEntry *> index_entry_map_{};
    HashMap<String, ChunkIndexEntry *> chunk_index_entries_{};

    Vector<Tuple<SegmentIndexEntry *, ChunkIndexEntry *, Vector<ChunkIndexEntry *>>> optimize_data_;

    enum struct NewTxnStoreStatus {
        kNone,
        kOptimizing,
    };
    NewTxnStoreStatus status_{NewTxnStoreStatus::kNone};
};

export struct NewTxnCompactStore {
    Vector<Pair<NewTxnSegmentStore, Vector<SegmentEntry *>>> compact_data_;

    CompactStatementType type_;

    NewTxnCompactStore();
    NewTxnCompactStore(CompactStatementType type);
};

export class NewTxnTableStore {
public:
    explicit inline NewTxnTableStore(NewTxn *txn, const String& table_name) : txn_(txn), table_name_(table_name) {}

    Tuple<UniquePtr<String>, Status> Import(SharedPtr<SegmentEntry> segment_entry, NewTxn *txn);

    Tuple<UniquePtr<String>, Status> Append(const SharedPtr<DataBlock> &input_block);

    void AddIndexStore(TableIndexEntry *table_index_entry);

    void AddSegmentIndexesStore(TableIndexEntry *table_index_entry, const Vector<SegmentIndexEntry *> &segment_index_entries);

    void AddChunkIndexStore(TableIndexEntry *table_index_entry, ChunkIndexEntry *chunk_index_entry);

    NewTxnIndexStore *GetIndexStore(TableIndexEntry *table_index_entry, bool need_lock);

    void DropIndexStore(TableIndexEntry *table_index_entry);

    Tuple<UniquePtr<String>, Status> Delete(const Vector<RowID> &row_ids);

    void SetCompactType(CompactStatementType type);

    Tuple<UniquePtr<String>, Status> Compact(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type);

    void AddSegmentStore(SegmentEntry *segment_entry);

    void AddBlockStore(SegmentEntry *segment_entry, BlockEntry *block_entry);

    void AddBlockColumnStore(SegmentEntry *segment_entry, BlockEntry *block_entry, BlockColumnEntry *block_column_entry);

    void AddSealedSegment(SegmentEntry *segment_entry);

    void AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnManager *txn_mgr, TxnTimeStamp commit_ts, bool added) const;

public:
    // transaction related

    void Rollback(TransactionID txn_id, TxnTimeStamp abort_ts);

    bool CheckConflict(Catalog *catalog, NewTxn *txn) const;

    Optional<String> CheckConflict(const NewTxnTableStore *txn_table_store) const;

    void PrepareCommit1(const Vector<WalSegmentInfo *> &segment_infos) const;

    void PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr);

    void Commit(TransactionID txn_id, TxnTimeStamp commit_ts);

    void MaintainCompactionAlg();

public: // Setter, Getter
    Pair<std::shared_lock<std::shared_mutex>, const HashMap<String, UniquePtr<NewTxnIndexStore>> &> txn_indexes_store() const;

    const HashMap<SegmentID, NewTxnSegmentStore> &txn_segments() const { return txn_segments_store_; }

    const Vector<SegmentEntry *> &flushed_segments() const { return flushed_segments_; }

    NewTxn *GetTxn() const { return txn_; }

    TableEntry *GetTableEntry() const { return table_entry_; }

    inline bool HasUpdate() const { return has_update_; }

    DeleteState &GetDeleteStateRef() { return delete_state_; }

    inline DeleteState *GetDeleteStatePtr() { return &delete_state_; }

    // inline const Vector<SharedPtr<DataBlock>> &GetBlocks() const { return blocks_; }

    // void SetAppendState(UniquePtr<AppendState> append_state) { append_state_ = std::move(append_state); }

    // inline AppendState *GetAppendState() const {
    //     LOG_INFO(fmt::format("CCC: {}", sizeof(*append_state_)));
    //     return append_state_.get();
    // }

    void AddWriteTxnNum() { added_txn_num_ = true; }

    bool AddedTxnNum() const { return added_txn_num_; }

private:
    mutable std::shared_mutex txn_table_store_mtx_{};

    HashMap<SegmentID, NewTxnSegmentStore> txn_segments_store_{};
    Vector<SegmentEntry *> flushed_segments_{};
    HashSet<SegmentEntry *> set_sealed_segments_{};

    int ptr_seq_n_;
    HashMap<TableIndexEntry *, int> txn_indexes_{};
    HashMap<String, UniquePtr<NewTxnIndexStore>> txn_indexes_store_{};

    NewTxnCompactStore compact_state_;

    NewTxn *const txn_{};
    // Vector<SharedPtr<DataBlock>> blocks_{};

public:
    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};

private:
    // SizeT current_block_id_{0};

    TableEntry *table_entry_{};
    bool added_txn_num_{false};

    bool has_update_{false};

    String table_name_;
public:
    void SetCompacting() { table_status_ = NewTxnStoreStatus::kCompacting; }

    void SetCreatingIndex() { table_status_ = NewTxnStoreStatus::kCreatingIndex; }

    void TryRevert();

private:
    enum struct NewTxnStoreStatus {
        kNone = 0,
        kCreatingIndex,
        kCompacting,
    };
    NewTxnStoreStatus table_status_{NewTxnStoreStatus::kNone};

public:
    Status SetColumnDefs(const String &db_name, const String &table_name);

    const Vector<SharedPtr<ColumnDef>> &column_defs() const { return column_defs_; }

private:
    Vector<SharedPtr<ColumnDef>> column_defs_;
};

export class NewTxnStore {
public:
    explicit NewTxnStore(NewTxn *txn);

    void AddDBStore(DBEntry *db_entry);

    void DropDBStore(DBEntry *dropped_db_entry);

    void AddTableStore(TableEntry *table_entry);

    void DropTableStore(TableEntry *dropped_table_entry);

    NewTxnTableStore *GetNewTxnTableStore(const String& table_name);

    NewTxnTableStore *GetExistNewTxnTableStore(TableEntry *table_entry) const;

    void AddDeltaOp(CatalogDeltaEntry *local_delta_opsm, TxnManager *txn_mgr) const;

    void MaintainCompactionAlg() const;

    bool CheckConflict(Catalog *catalog);

    Optional<String> CheckConflict(const NewTxnStore &txn_store);

    void PrepareCommit1();

    void PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr);

    void CommitBottom(TransactionID txn_id, TxnTimeStamp commit_ts);

    void Rollback(TransactionID txn_id, TxnTimeStamp abort_ts);

    bool ReadOnly() const;

    std::mutex mtx_{};

    void RevertTableStatus();

    void SetCompacting(TableEntry *table_entry);

    void SetCreatingIndex(TableEntry *table_entry);

    void AddSemaphore(UniquePtr<std::binary_semaphore> sema) { semas_.push_back(std::move(sema)); }

    const Vector<UniquePtr<std::binary_semaphore>> &semas() const { return semas_; }

private:
    // Txn store
    NewTxn *txn_{}; // TODO: remove this
    int ptr_seq_n_{};
    HashMap<DBEntry *, int> txn_dbs_{};
    HashMap<TableEntry *, int> txn_tables_{};
    // Key: table name Value: NewTxnTableStore
    HashMap<String, UniquePtr<NewTxnTableStore>> txn_tables_store_{};

    Vector<UniquePtr<std::binary_semaphore>> semas_{};

public:
    WalCmdDumpIndex *GetDumpIndexCmd(const String &idx_segment_key);

    bool AddDumpIndexCmd(String idx_segment_key, WalCmdDumpIndex *dump_index_cmd);

private:
    HashMap<String, WalCmdDumpIndex *> dump_index_cmds_{};
};

} // namespace infinity
