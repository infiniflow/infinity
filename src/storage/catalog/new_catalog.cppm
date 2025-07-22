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
import status;
import meta_info;
import extra_ddl_info;
import default_values;
import internal_types;
import buffer_handle;
import column_def;
import profiler;
import third_party;
import storage;
import meta_tree;

namespace infinity {

class NewTxn;
struct MemIndex;
class TableIndexReaderCache;
class DBMeeta;
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
class KVStore;
class KVInstance;
class TableDef;
class IndexBase;

struct WalSegmentInfo;
struct WalBlockInfo;
struct WalChunkIndexInfo;
class Config;
struct MemIndexID;
class TableCache;
class DbCache;
class SystemCache;
class FunctionSet;
class SpecialFunction;

enum class ColumnVectorMode;

template <bool init_all_true>
struct RoaringBitmap;
using Bitmask = RoaringBitmap<true>;

export struct TableLockForMemIndex {
    std::mutex mtx_;
    bool dumping_mem_index_ = false;
    SizeT append_count_{0};
};

export struct BlockLock {
    BlockLock() = default;
    BlockLock(TxnTimeStamp checkpoint_ts) : checkpoint_ts_(checkpoint_ts) {}

    std::shared_mutex mtx_;
    TxnTimeStamp min_ts_{};
    TxnTimeStamp max_ts_{};
    TxnTimeStamp checkpoint_ts_{};
};

export struct SegmentIndexFtInfo {
    u64 ft_column_len_sum_{}; // increase only
    u32 ft_column_len_cnt_{}; // increase only
};

export struct SegmentUpdateTS {
    std::shared_mutex mtx_;
    TxnTimeStamp ts_{0};
};

struct ChunkInfoForCreateIndex;

export class NewTxnGetVisibleRangeState {
public:
    NewTxnGetVisibleRangeState() = default;

    void Init(SharedPtr<BlockLock> block_lock, BufferHandle version_buffer_handle, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts_);

    bool Next(BlockOffset block_offset_begin, Pair<BlockOffset, BlockOffset> &visible_range);

    bool Next(Pair<BlockOffset, BlockOffset> &visible_range) { return Next(block_offset_begin_, visible_range); }

    void SetBlockOffsetBegin(BlockOffset block_offset_begin) { block_offset_begin_ = block_offset_begin; }

    BlockOffset block_offset_end() const { return block_offset_end_; }

    bool end() const { return end_; }

private:
    SharedPtr<BlockLock> block_lock_;
    BufferHandle version_buffer_handle_;
    TxnTimeStamp begin_ts_ = 0;
    TxnTimeStamp commit_ts_ = 0;
    BlockOffset block_offset_begin_ = 0;
    BlockOffset block_offset_end_ = 0;
    bool end_ = false;
};

export class NewTxnBlockVisitor {
public:
    NewTxnBlockVisitor(NewTxnGetVisibleRangeState *visit_state) : visit_state_(visit_state) {}

    Optional<BlockOffset> Next();

    BlockOffset cur() const { return cur_; }

private:
    NewTxnGetVisibleRangeState *visit_state_ = nullptr;
    Pair<BlockOffset, BlockOffset> visible_range_ = {0, 0};
    BlockOffset cur_ = 0;
    bool end_ = false;
};

// This enumerates type is to indicate which case the meta objects are used to.
export enum class UsageFlag {
    kTransform, // Used by catalog transformation from old json style to rocksdb.
    kOther,     // Other cases
};

export struct NewCatalog {
public:
    explicit NewCatalog(KVStore *kv_store);
    virtual ~NewCatalog();
    static Status Init(KVStore *kv_store);

public:
    SharedPtr<MetaTree> MakeMetaTree() const;
    Vector<SharedPtr<MetaKey>> MakeMetaKeys() const;
    UniquePtr<SystemCache> RestoreCatalogCache(Storage *storage_ptr);

    KVStore *kv_store() const;

private:
    KVStore *kv_store_{};

    mutable std::mutex catalog_cache_mtx_{};

    HashMap<u64, SharedPtr<HashMap<u64, SharedPtr<TableCache>>>> table_cache_map_{};

public:
    Status AddBlockLock(String block_key);
    Status AddBlockLock(String block_key, TxnTimeStamp checkpoint_ts);
    Status GetBlockLock(const String &block_key, SharedPtr<BlockLock> &block_lock);
    Status DropBlockLockByBlockKey(const String &block_key);

private:
    std::shared_mutex block_lock_mtx_{};
    HashMap<String, SharedPtr<BlockLock>> block_lock_map_{};

public:
    SharedPtr<MemIndex> GetMemIndex(const String &mem_index_key);
    SharedPtr<MemIndex> PopMemIndex(const String &mem_index_key);
    bool HasMemIndex(const String &mem_index_key);
    Status DropMemIndexByMemIndexKey(const String &mem_index_key);
    Vector<Pair<String, String>> GetAllMemIndexInfo();

private:
    mutable std::shared_mutex mem_index_mtx_{};
    HashMap<String, SharedPtr<MemIndex>> mem_index_map_{};

public:
    Status AddFtIndexCache(String ft_index_cache_key, SharedPtr<TableIndexReaderCache> ft_index_cache);
    Status GetFtIndexCache(const String &ft_index_cache_key, SharedPtr<TableIndexReaderCache> &ft_index_cache);
    Status DropFtIndexCacheByFtIndexCacheKey(const String &ft_index_cache_key);

private:
    std::shared_mutex ft_index_cache_mtx_{};
    HashMap<String, SharedPtr<TableIndexReaderCache>> ft_index_cache_map_{};

public:
    Status AddSegmentUpdateTS(String segment_update_ts_key, SharedPtr<SegmentUpdateTS> segment_update_ts);
    Status GetSegmentUpdateTS(const String &segment_update_ts_key, SharedPtr<SegmentUpdateTS> &segment_update_ts);
    void DropSegmentUpdateTSByKey(const String &segment_update_ts_key);

private:
    std::shared_mutex segment_update_ts_mtx_{};
    HashMap<String, SharedPtr<SegmentUpdateTS>> segment_update_ts_map_{};

public:
    // Currently, these function or function set can't be changed and also will not be persistent.
    HashMap<String, SharedPtr<FunctionSet>> function_sets_{};
    HashMap<String, SharedPtr<SpecialFunction>> special_functions_{};
    HashMap<String, UniquePtr<ColumnDef>> special_columns_{};

public:
    Status GetCleanedMeta(TxnTimeStamp ts, KVInstance *kv_instance, Vector<UniquePtr<MetaKey>> &metas, Vector<String> &drop_keys) const;
    Vector<String> GetEncodeKeys(Vector<UniquePtr<MetaKey>> &metas) const;

    // Profile related methods
    void SetProfile(bool flag) { enable_profile_ = flag; }

    [[nodiscard]] bool GetProfile() const { return enable_profile_; }

    void AppendProfileRecord(SharedPtr<QueryProfiler> profiler) { history_.Enqueue(std::move(profiler)); }

    const QueryProfiler *GetProfileRecord(SizeT index) { return history_.GetElement(index); }

    Vector<SharedPtr<QueryProfiler>> GetProfileRecords() { return history_.GetElements(); }

    void ResizeProfileHistory(SizeT new_size) { history_.Resize(new_size); }

    SizeT ProfileHistorySize() const { return history_.HistoryCapacity(); }

    void SetLastCleanupTS(TxnTimeStamp cleanup_ts);
    TxnTimeStamp GetLastCleanupTS() const;

private:
    ProfileHistory history_{DEFAULT_PROFILER_HISTORY_SIZE};
    atomic_bool enable_profile_{false};
    // bool is_vfs_{false};
    Atomic<TxnTimeStamp> last_cleanup_ts_{0};

public:
    static Status InitCatalog(KVInstance *kv_instance, TxnTimeStamp checkpoint_ts);

    static Status MemIndexRecover(NewTxn *txn);

    static Status MemIndexCommit(NewTxn *txn);

    static Status GetAllMemIndexes(NewTxn *txn, Vector<SharedPtr<MemIndex>> &mem_indexes, Vector<MemIndexID> &mem_index_ids);

    static Status AddNewDB(NewTxn *txn,
                           const String &db_id_str,
                           TxnTimeStamp commit_ts,
                           const String &db_name,
                           const String *db_comment,
                           Optional<DBMeeta> &db_meta);

    static Status CleanDB(DBMeeta &db_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    static Status AddNewTable(DBMeeta &db_meta,
                              const String &table_id_str,
                              TxnTimeStamp begin_ts,
                              TxnTimeStamp commit_ts,
                              const SharedPtr<TableDef> &table_def,
                              Optional<TableMeeta> &table_meta);

    static Status CleanTable(TableMeeta &table_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    Status AddNewTableIndex(TableMeeta &table_meta,
                            String &index_id_str,
                            TxnTimeStamp commit_ts,
                            const SharedPtr<IndexBase> &index_base,
                            Optional<TableIndexMeeta> &table_index_meta);

    static Status CleanTableIndex(TableIndexMeeta &table_index_meta, UsageFlag usage_flag);

    // static Status AddNewSegment(TableMeeta &table_meta, SegmentID segment_id, Optional<SegmentMeta> &segment_meta);

    static Status AddNewSegment1(TableMeeta &table_meta, TxnTimeStamp commit_ts, Optional<SegmentMeta> &segment_meta);

    static Status AddNewSegmentWithID(TableMeeta &table_meta, TxnTimeStamp commit_ts, Optional<SegmentMeta> &segment_meta, SegmentID segment_id);

    static Status LoadFlushedSegment1(TableMeeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp checkpoint_ts);

    static Status LoadFlushedSegment2(TableMeeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp checkpoint_ts);

    static Status CleanSegment(SegmentMeta &segment_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    // static Status AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, Optional<BlockMeta> &block_meta);

    static Status AddNewBlock1(SegmentMeta &segment_meta, TxnTimeStamp commit_ts, Optional<BlockMeta> &block_meta);

    static Status LoadImportedOrCompactedSegment(TableMeeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp commit_ts);

    static Status AddNewBlockWithID(SegmentMeta &segment_meta, TxnTimeStamp commit_ts, Optional<BlockMeta> &block_meta, BlockID block_id);

    static Status AddNewBlockForTransform(SegmentMeta &segment_meta, TxnTimeStamp commit_ts, Optional<BlockMeta> &block_meta);

    static Status LoadFlushedBlock1(SegmentMeta &segment_meta, const WalBlockInfo &block_info, TxnTimeStamp checkpoint_ts);

    static Status CleanBlock(BlockMeta &block_meta, UsageFlag usage_flag);

    static Status AddNewBlockColumn(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta);

    static Status AddNewBlockColumnForTransform(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta, TxnTimeStamp commit_ts);

    static Status CleanBlockColumn(ColumnMeta &column_meta, const ColumnDef *column_def, UsageFlag usage_flag);

    static Status AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta);

    static Status
    AddNewSegmentIndex1(TableIndexMeeta &table_index_meta, NewTxn *new_txn, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta);

    static Status CleanSegmentIndex(SegmentIndexMeta &segment_index_meta, UsageFlag usage_flag);

    static Status AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
                                   ChunkID chunk_id,
                                   RowID base_row_id,
                                   SizeT row_count,
                                   const String &base_name,
                                   SizeT index_size,
                                   Optional<ChunkIndexMeta> &chunk_index_meta);

    static Status AddNewChunkIndex1(SegmentIndexMeta &segment_index_meta,
                                    NewTxn *new_txn,
                                    ChunkID chunk_id,
                                    RowID base_row_id,
                                    SizeT row_count,
                                    const String &base_name,
                                    SizeT index_size,
                                    Optional<ChunkIndexMeta> &chunk_index_meta);

    static Status LoadFlushedChunkIndex(SegmentIndexMeta &segment_index_meta, const WalChunkIndexInfo &chunk_info);

    static Status LoadFlushedChunkIndex1(SegmentIndexMeta &segment_index_meta, const WalChunkIndexInfo &chunk_info, NewTxn *new_txn);

    static Status CleanChunkIndex(ChunkIndexMeta &chunk_index_meta, UsageFlag usage_flag);

    static Status GetColumnVector(ColumnMeta &column_meta, SizeT row_count, const ColumnVectorMode &tipe, ColumnVector &column_vector);

    static Status GetBlockVisibleRange(BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, NewTxnGetVisibleRangeState &state);

    static Status GetCreateTSVector(BlockMeta &block_meta, SizeT offset, SizeT row_count, ColumnVector &column_vector);

    static Status GetDeleteTSVector(BlockMeta &block_meta, SizeT offset, SizeT row_count, ColumnVector &column_vector);

    static Status GetDBFilePaths(TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, DBMeeta &db_meta, Vector<String> &file_paths);

    static Status
    GetTableFilePaths(TxnTimeStamp begin_ts, TableMeeta &table_meta, Vector<String> &file_paths, SharedPtr<ColumnDef> column_def = nullptr);

    static Status
    GetSegmentFilePaths(TxnTimeStamp begin_ts, SegmentMeta &segment_meta, Vector<String> &file_paths, SharedPtr<ColumnDef> column_def = nullptr);

    static Status GetBlockFilePaths(BlockMeta &block_meta, Vector<String> &file_paths, SharedPtr<ColumnDef> column_def = nullptr);

    static Status GetBlockColumnFilePaths(ColumnMeta &column_meta, Vector<String> &file_paths);

    static Status GetColumnFilePaths(TxnTimeStamp begin_ts, TableMeeta &table_meta, SharedPtr<ColumnDef> column_def, Vector<String> &file_paths);

    static Status GetTableIndexFilePaths(TableIndexMeeta &table_index_meta, Vector<String> &file_paths);

    static Status GetSegmentIndexFilepaths(SegmentIndexMeta &segment_index_meta, Vector<String> &file_paths);

    static Status GetChunkIndexFilePaths(ChunkIndexMeta &chunk_index_meta, Vector<String> &file_paths);

    static Status CheckColumnIfIndexed(TableMeeta &table_meta, ColumnID column_id, bool &has_index);

    static Status CheckTableIfDelete(TableMeeta &table_meta, TxnTimeStamp begin_ts, bool &has_delete);

    static Status SetBlockDeleteBitmask(BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, Bitmask &bitmask);

    static Status CheckSegmentRowsVisible(SegmentMeta &segment_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, Bitmask &bitmask);

public:
    // Function related methods
    static SharedPtr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, String function_name);

    static void AddFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void AppendToScalarFunctionSet(NewCatalog *catalog, const SharedPtr<FunctionSet> &function_set);

    static void AddSpecialFunction(NewCatalog *catalog, const SharedPtr<SpecialFunction> &special_function);

    static Tuple<SpecialFunction *, Status> GetSpecialFunctionByNameNoExcept(NewCatalog *catalog, String function_name);
};
} // namespace infinity
