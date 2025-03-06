// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module new_catalog;

import stl;
import table_def;
import status;
import meta_info;
import extra_ddl_info;
import kv_store;
import default_values;
import internal_types;
import buffer_handle;

namespace infinity {

class NewTxn;
class MemIndex;
class TableIndexReaderCache;
class TableMeeta;
class SegmentMeta;
class BlockMeta;
class ColumnMeta;
class TableIndexMeeta;
class SegmentIndexMeta;
class ChunkIndexMeta;
class BufferObj;
class ColumnVector;
struct MetaKey;

enum class ColumnVectorTipe;

export enum class LockType { kLocking, kLocked, kUnlocking, kUnlocked, kImmutable };

export struct TableMemoryContext {
    LockType locker{LockType::kUnlocked};
    TransactionID locked_txn_{MAX_TXN_ID};
    SizeT write_txn_num_{0};
};

export struct BlockLock {
    std::shared_mutex mtx_;
};

export class NewTxnGetVisibleRangeState {
public:
    NewTxnGetVisibleRangeState() = default;

    void Init(SharedPtr<BlockLock> block_lock, BufferHandle version_buffer_handle, TxnTimeStamp begin_ts);

    bool Next(BlockOffset block_offset_begin, Pair<BlockOffset, BlockOffset> &visible_range);

    BlockOffset block_offset_end() const { return block_offset_end_; }

private:
    SharedPtr<BlockLock> block_lock_;
    BufferHandle version_buffer_handle_;
    TxnTimeStamp begin_ts_ = 0;
    BlockOffset block_offset_end_ = 0;
};

export struct NewCatalog {
public:
    explicit NewCatalog(KVStore *kv_store);
    ~NewCatalog();

public:
    // Database related functions
    Status CreateDatabase(const SharedPtr<String> &db_name,
                          const SharedPtr<String> &comment,
                          NewTxn *txn,
                          ConflictType conflict_type = ConflictType::kError);

    Status DropDatabase(const SharedPtr<String> &db_name, NewTxn *txn, ConflictType conflict_type = ConflictType::kError);

    bool CheckDatabaseExists(const SharedPtr<String> &db_name, NewTxn *txn);

    Tuple<SharedPtr<DatabaseInfo>, Status> GetDatabaseInfo(const String &db_name, NewTxn *txn);

    //    void RemoveDBEntry(DBEntry *db_entry, TransactionID txn_id);
    //
    //    // replay
    //    void
    //    CreateDatabaseReplay(const SharedPtr<String> &db_name,
    //                         const SharedPtr<String> &comment,
    //                         std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)>
    //                         &&init_entry, TransactionID txn_id, TxnTimeStamp begin_ts);
    //
    //    void DropDatabaseReplay(const String &db_name,
    //                            std::function<SharedPtr<DBEntry>(DBMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    //                            TransactionID txn_id,
    //                            TxnTimeStamp begin_ts);
    //
    //    DBEntry *GetDatabaseReplay(const String &db_name, TransactionID txn_id, TxnTimeStamp begin_ts);

    static String GetPathNameTail(const String &path);

public:
    Status LockTable(const String &table_key, TransactionID txn_id);
    Status CommitLockTable(const String &table_key, TransactionID txn_id);
    Status RollbackLockTable(const String &table_key, TransactionID txn_id);

    Status UnlockTable(const String &table_key, TransactionID txn_id);
    Status CommitUnlockTable(const String &table_key, TransactionID txn_id);
    Status RollbackUnlockTable(const String &table_key, TransactionID txn_id);

    Status ImmutateTable(const String &table_key, TransactionID txn_id);
    Status MutateTable(const String &table_key, TransactionID txn_id);

    Status IncreaseTableWriteCount(const String &table_key);
    Status DecreaseTableWriteCount(const String &table_key);

private:
    KVStore *kv_store_{};

private:
    mutable std::mutex mtx_{};
    HashMap<String, SharedPtr<TableMemoryContext>> table_memory_context_map_{};

public:
    Status AddBlockLock(String block_key);
    Status GetBlockLock(const String &block_key, SharedPtr<BlockLock> &block_lock);
    Status DropBlockLockByBlockKey(const String &block_key);

private:
    std::shared_mutex block_lock_mtx_{};
    HashMap<String, SharedPtr<BlockLock>> block_lock_map_{};

public:
    Status AddMemIndex(String mem_index_key, SharedPtr<MemIndex> mem_index);
    Status GetMemIndex(const String &mem_index_key, SharedPtr<MemIndex> &mem_index);
    Status DropMemIndexByMemIndexKey(const String &mem_index_key);

private:
    std::shared_mutex mem_index_mtx_{};
    HashMap<String, SharedPtr<MemIndex>> mem_index_map_{};

public:
    Status AddFtIndexCache(String ft_index_cache_key, SharedPtr<TableIndexReaderCache> ft_index_cache);
    Status GetFtIndexCache(const String &ft_index_cache_key, SharedPtr<TableIndexReaderCache> &ft_index_cache);
    Status DropFtIndexCacheByFtIndexCacheKey(const String &ft_index_cache_key);

private:
    std::shared_mutex ft_index_cache_mtx_{};
    HashMap<String, SharedPtr<TableIndexReaderCache>> ft_index_cache_map_{};

public:
    void AddCleanedMeta(TxnTimeStamp ts, UniquePtr<MetaKey> meta);

    void GetCleanedMeta(TxnTimeStamp ts, Vector<UniquePtr<MetaKey>> &metas);

private:
    std::mutex cleaned_meta_mtx_{};
    MultiMap<TxnTimeStamp, UniquePtr<MetaKey>> cleaned_meta_{};

public:
    static Status CleanTable(TableMeeta &table_meta);

    static Status CleanTableIndex(TableIndexMeeta &table_index_meta);

    static Status AddNewSegment(TableMeeta &table_meta, SegmentID segment_id, Optional<SegmentMeta> &segment_meta);

    static Status CleanSegment(SegmentMeta &segment_meta);

    static Status AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, Optional<BlockMeta> &block_meta);

    static Status CleanBlock(BlockMeta &block_meta);

    static Status AddNewBlockColumn(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta);

    static Status CleanBlockColumn(ColumnMeta &column_meta);

    static Status AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta);

    static Status CleanSegmentIndex(SegmentIndexMeta &segment_index_meta);

    static Status AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
                                   ChunkID chunk_id,
                                   RowID base_row_id,
                                   SizeT row_count,
                                   const String &base_name,
                                   SizeT index_size,
                                   Optional<ChunkIndexMeta> &chunk_index_meta,
                                   BufferObj *&buffer_obj);

    static Status CleanChunkIndex(ChunkIndexMeta &chunk_index_meta);

    static Status GetColumnVector(ColumnMeta &column_meta, SizeT row_count, const ColumnVectorTipe &tipe, ColumnVector &column_vector);

    static Status GetBlockVisibleRange(BlockMeta &block_meta, TxnTimeStamp begin_ts, NewTxnGetVisibleRangeState &state);

    static Status GetChunkIndex(ChunkIndexMeta &chunk_index_meta, BufferObj *&buffer_obj);

    static Status GetColumnBufferObj(ColumnMeta &column_meta, BufferObj *&buffer_obj, BufferObj *&outline_buffer_obj);

    static Status GetVersionBufferObj(BlockMeta &block_meta, BufferObj *&buffer_obj);
};

} // namespace infinity