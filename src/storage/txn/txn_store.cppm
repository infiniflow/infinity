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

export module txn_store;

import stl;

import data_access_state;
import status;
import internal_types;
import index_base;
import extra_ddl_info;

namespace infinity {

class Txn;
struct Catalog;
struct DBEntry;
struct TableIndexEntry;
struct TableEntry;
struct SegmentEntry;
struct BlockEntry;
class DataBlock;
class SegmentIndexEntry;
class ChunkIndexEntry;
class BGTaskProcessor;
class TxnManager;
enum class CompactStatementType;
class CatalogDeltaEntry;
class BufferManager;

export struct TxnSegmentStore {
public:
    static TxnSegmentStore AddSegmentStore(SegmentEntry *segment_entry);

    explicit TxnSegmentStore(SegmentEntry *segment_entry);

    TxnSegmentStore() = default;

    void AddDeltaOp(CatalogDeltaEntry *local_delta_ops, AppendState *append_state, Txn *txn, bool set_sealed) const;

public:
    SegmentEntry *const segment_entry_ = nullptr;
    HashMap<BlockID, BlockEntry *> block_entries_;
};

export struct TxnIndexStore {
public:
    explicit TxnIndexStore(TableIndexEntry *table_index_entry);
    TxnIndexStore() = default;

    void AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnTimeStamp commit_ts) const;

    void PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr);

    void Commit(TransactionID txn_id, TxnTimeStamp commit_ts) const;

public:
    TableIndexEntry *const table_index_entry_{};

    HashMap<SegmentID, SegmentIndexEntry *> index_entry_map_{};
    Vector<ChunkIndexEntry *> chunk_index_entries_{};

    Vector<Tuple<SegmentIndexEntry *, ChunkIndexEntry *, Vector<ChunkIndexEntry *>>> optimize_data_;
};

export struct TxnCompactStore {
    Vector<Pair<TxnSegmentStore, Vector<SegmentEntry *>>> compact_data_;

    CompactStatementType type_;

    TxnCompactStore();
    TxnCompactStore(CompactStatementType type);
};

export class TxnTableStore {
public:
    explicit inline TxnTableStore(Txn *txn, TableEntry *table_entry) : txn_(txn), table_entry_(table_entry) {}

    Tuple<UniquePtr<String>, Status> Import(SharedPtr<SegmentEntry> segment_entry, Txn *txn);

    Tuple<UniquePtr<String>, Status> Append(const SharedPtr<DataBlock> &input_block);

    void AddIndexStore(TableIndexEntry *table_index_entry);

    void AddSegmentIndexesStore(TableIndexEntry *table_index_entry, const Vector<SegmentIndexEntry *> &segment_index_entries);

    void AddChunkIndexStore(TableIndexEntry *table_index_entry, ChunkIndexEntry *chunk_index_entry);

    TxnIndexStore *GetIndexStore(TableIndexEntry *table_index_entry);

    void DropIndexStore(TableIndexEntry *table_index_entry);

    Tuple<UniquePtr<String>, Status> Delete(const Vector<RowID> &row_ids);

    Tuple<UniquePtr<String>, Status> Compact(Vector<Pair<SharedPtr<SegmentEntry>, Vector<SegmentEntry *>>> &&segment_data, CompactStatementType type);

    void Rollback(TransactionID txn_id, TxnTimeStamp abort_ts);

    bool CheckConflict(const TxnTableStore *txn_table_store) const;

    void PrepareCommit1();

    void PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr);

    void Commit(TransactionID txn_id, TxnTimeStamp commit_ts) const;

    void MaintainCompactionAlg() const;

    void AddSegmentStore(SegmentEntry *segment_entry);

    void AddBlockStore(SegmentEntry *segment_entry, BlockEntry *block_entry);

    void AddSealedSegment(SegmentEntry *segment_entry);

    void AddDeltaOp(CatalogDeltaEntry *local_delta_ops, TxnManager *txn_mgr, TxnTimeStamp commit_ts, bool added) const;

public: // Getter
    const HashMap<String, UniquePtr<TxnIndexStore>> &txn_indexes_store() const { return txn_indexes_store_; }

    const HashMap<SegmentID, TxnSegmentStore> &txn_segments() const { return txn_segments_store_; }

    const Vector<SegmentEntry *> &flushed_segments() const { return flushed_segments_; }

private:
    HashMap<SegmentID, TxnSegmentStore> txn_segments_store_{};
    Vector<SegmentEntry *> flushed_segments_{};
    HashSet<SegmentEntry *> set_sealed_segments_{};

    int ptr_seq_n_;
    HashMap<TableIndexEntry *, int> txn_indexes_{};
    HashMap<String, UniquePtr<TxnIndexStore>> txn_indexes_store_{};

    TxnCompactStore compact_state_;

public:
    Txn *const txn_{};
    Vector<SharedPtr<DataBlock>> blocks_{};

    UniquePtr<AppendState> append_state_{};
    DeleteState delete_state_{};

    SizeT current_block_id_{0};

    TableEntry *table_entry_{};
};

export class TxnStore {
public:
    TxnStore(Txn *txn, Catalog *catalog);

    void AddDBStore(DBEntry *db_entry);

    void DropDBStore(DBEntry *dropped_db_entry);

    void AddTableStore(TableEntry *table_entry);

    void DropTableStore(TableEntry *dropped_table_entry);

    TxnTableStore *GetTxnTableStore(TableEntry *table_entry);

    void AddDeltaOp(CatalogDeltaEntry *local_delta_opsm, TxnManager *txn_mgr) const;

    void MaintainCompactionAlg() const;

    bool CheckConflict(const TxnStore &txn_store);

    void PrepareCommit1();

    void PrepareCommit(TransactionID txn_id, TxnTimeStamp commit_ts, BufferManager *buffer_mgr);

    void CommitBottom(TransactionID txn_id, TxnTimeStamp commit_ts);

    void Rollback(TransactionID txn_id, TxnTimeStamp abort_ts);

    bool Empty() const;

private:
    // Txn store
    Txn *txn_{}; // TODO: remove this
    Catalog *catalog_{};
    int ptr_seq_n_{};
    HashMap<DBEntry *, int> txn_dbs_{};
    HashMap<TableEntry *, int> txn_tables_{};
    // Key: table name Value: TxnTableStore
    HashMap<String, SharedPtr<TxnTableStore>> txn_tables_store_{};
};

} // namespace infinity
