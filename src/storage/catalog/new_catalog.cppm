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

export module infinity_core:new_catalog;

import :status;
import :meta_info;
import :default_values;
import :profiler;
import :storage;
import :meta_tree;

import third_party;

import column_def;
import extra_ddl_info;
import internal_types;

namespace infinity {

class NewTxn;
struct MemIndex;
class TableIndexReaderCache;
export class DBMeta;
export class TableMeta;
export class SegmentMeta;
export class BlockMeta;
export class ColumnMeta;
export class TableIndexMeta;
export class SegmentIndexMeta;
export class ChunkIndexMeta;
export struct ColumnVector;
struct MetaKey;
class KVStore;
class KVInstance;
class TableDef;
class IndexBase;

struct WalSegmentInfo;
struct WalBlockInfo;
struct WalChunkIndexInfo;
struct Config;
struct MemIndexID;
class TableCache;
class DbCache;
class SystemCache;
class FunctionSet;
class SpecialFunction;
class MetaCache;

enum class ColumnVectorMode;

template <bool init_all_true>
struct RoaringBitmap;
using Bitmask = RoaringBitmap<true>;

export struct TableLockForMemIndex {
    std::mutex mtx_;
    bool dumping_mem_index_ = false;
    size_t append_count_{0};
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

    void Init(std::shared_ptr<BlockLock> block_lock, FileWorker *version_buffer_obj, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts_);

    bool Next(BlockOffset block_offset_begin, std::pair<BlockOffset, BlockOffset> &visible_range);

    bool Next(std::pair<BlockOffset, BlockOffset> &visible_range) { return Next(block_offset_begin_, visible_range); }

    void SetBlockOffsetBegin(BlockOffset block_offset_begin) { block_offset_begin_ = block_offset_begin; }

    BlockOffset block_offset_end() const { return block_offset_end_; }

    bool end() const { return end_; }

private:
    std::shared_ptr<BlockLock> block_lock_;
    FileWorker *version_buffer_obj_;
    TxnTimeStamp begin_ts_ = 0;
    TxnTimeStamp commit_ts_ = 0;
    BlockOffset block_offset_begin_ = 0;
    BlockOffset block_offset_end_ = 0;
    bool end_ = false;
};

export class NewTxnBlockVisitor {
public:
    NewTxnBlockVisitor(NewTxnGetVisibleRangeState *visit_state) : visit_state_(visit_state) {}

    std::optional<BlockOffset> Next();

    BlockOffset cur() const { return cur_; }

private:
    NewTxnGetVisibleRangeState *visit_state_ = nullptr;
    std::pair<BlockOffset, BlockOffset> visible_range_ = {0, 0};
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
    std::shared_ptr<MetaTree> MakeMetaTree() const;
    std::vector<std::shared_ptr<MetaKey>> MakeMetaKeys() const;
    std::unique_ptr<SystemCache> RestoreCatalogCache(Storage *storage_ptr);

    KVStore *kv_store() const;

private:
    KVStore *kv_store_{};

    mutable std::mutex catalog_cache_mtx_{};

    std::unordered_map<u64, std::shared_ptr<std::unordered_map<u64, std::shared_ptr<TableCache>>>> table_cache_map_{};

public:
    Status AddBlockLock(std::string block_key);
    Status AddBlockLock(std::string block_key, TxnTimeStamp checkpoint_ts);
    Status GetBlockLock(const std::string &block_key, std::shared_ptr<BlockLock> &block_lock);
    Status DropBlockLockByBlockKey(const std::string &block_key);

private:
    std::shared_mutex block_lock_mtx_{};
    std::unordered_map<std::string, std::shared_ptr<BlockLock>> block_lock_map_{};

public:
    std::shared_ptr<MemIndex> GetMemIndex(const std::string &mem_index_key, bool for_update);
    std::shared_ptr<MemIndex> PopMemIndex(const std::string &mem_index_key);
    Status DropMemIndexByMemIndexKey(const std::string &mem_index_key);
    std::vector<std::pair<std::string, std::string>> GetAllMemIndexInfo();

private:
    mutable std::shared_mutex mem_index_mtx_{};
    std::unordered_map<std::string, std::shared_ptr<MemIndex>> mem_index_map_{};

public:
    Status AddFtIndexCache(std::string ft_index_cache_key, std::shared_ptr<TableIndexReaderCache> ft_index_cache);
    Status GetFtIndexCache(const std::string &ft_index_cache_key, std::shared_ptr<TableIndexReaderCache> &ft_index_cache);
    Status DropFtIndexCacheByFtIndexCacheKey(const std::string &ft_index_cache_key);

private:
    std::shared_mutex ft_index_cache_mtx_{};
    std::unordered_map<std::string, std::shared_ptr<TableIndexReaderCache>> ft_index_cache_map_{};

    std::shared_mutex segment_update_ts_mtx_{};
    std::unordered_map<std::string, std::shared_ptr<SegmentUpdateTS>> segment_update_ts_map_{};

public:
    // Currently, these function or function set can't be changed and also will not be persistent.
    std::unordered_map<std::string, std::shared_ptr<FunctionSet>> function_sets_{};
    std::unordered_map<std::string, std::shared_ptr<SpecialFunction>> special_functions_{};
    std::unordered_map<std::string, std::unique_ptr<ColumnDef>> special_columns_{};

public:
    static Status
    GetCleanedMeta(TxnTimeStamp ts, KVInstance *kv_instance, std::vector<std::shared_ptr<MetaKey>> &metas, std::vector<std::string> &drop_keys);
    std::vector<std::string> GetEncodeKeys(std::vector<std::shared_ptr<MetaKey>> &metas) const;

    // Profile related methods
    void SetProfile(bool flag) { enable_profile_ = flag; }

    [[nodiscard]] bool GetProfile() const { return enable_profile_; }

    void AppendProfileRecord(std::shared_ptr<QueryProfiler> profiler) { history_.Enqueue(std::move(profiler)); }

    const QueryProfiler *GetProfileRecord(size_t index) { return history_.GetElement(index); }

    std::vector<std::shared_ptr<QueryProfiler>> GetProfileRecords() { return history_.GetElements(); }

    void ResizeProfileHistory(size_t new_size) { history_.Resize(new_size); }

    size_t ProfileHistorySize() const { return history_.HistoryCapacity(); }

    void SetLastCleanupTS(TxnTimeStamp cleanup_ts);
    TxnTimeStamp GetLastCleanupTS() const;

private:
    ProfileHistory history_{DEFAULT_PROFILER_HISTORY_SIZE};
    std::atomic_bool enable_profile_{false};
    // bool is_vfs_{false};
    std::atomic<TxnTimeStamp> last_cleanup_ts_{0};

public:
    static Status InitCatalog(MetaCache *meta_cache, KVInstance *kv_instance, TxnTimeStamp checkpoint_ts);

    static Status MemIndexRecover(NewTxn *txn);

    static Status GetAllMemIndexes(NewTxn *txn, std::vector<std::shared_ptr<MemIndex>> &mem_indexes, std::vector<MemIndexID> &mem_index_ids);

    static Status AddNewDB(NewTxn *txn,
                           const std::string &db_id_str,
                           TxnTimeStamp commit_ts,
                           const std::string &db_name,
                           const std::string *db_comment,
                           std::shared_ptr<DBMeta> &db_meta);

    static Status CleanDB(DBMeta &db_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    static Status AddNewTable(DBMeta &db_meta,
                              const std::string &table_id_str,
                              TxnTimeStamp begin_ts,
                              TxnTimeStamp commit_ts,
                              const std::shared_ptr<TableDef> &table_def,
                              std::shared_ptr<TableMeta> &table_meta);

    static Status CleanTable(TableMeta &table_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    Status AddNewTableIndex(TableMeta &table_meta,
                            const std::string &index_id_str,
                            TxnTimeStamp commit_ts,
                            const std::shared_ptr<IndexBase> &index_base,
                            std::shared_ptr<TableIndexMeta> &table_index_meta);

    static Status CleanTableIndex(TableIndexMeta &table_index_meta, UsageFlag usage_flag);

    static Status AddNewSegmentWithID(TableMeta &table_meta, TxnTimeStamp commit_ts, std::optional<SegmentMeta> &segment_meta, SegmentID segment_id);

    static Status LoadFlushedSegment2(TableMeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp checkpoint_ts);

    static Status CleanSegment(SegmentMeta &segment_meta, TxnTimeStamp begin_ts, UsageFlag usage_flag);

    // static Status AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, std::optional<BlockMeta> &block_meta);

    static Status AddNewBlock1(SegmentMeta &segment_meta, TxnTimeStamp commit_ts, std::optional<BlockMeta> &block_meta);

    static Status LoadImportedOrCompactedSegment(TableMeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp commit_ts);

    static Status AddNewBlockWithID(SegmentMeta &segment_meta, TxnTimeStamp commit_ts, std::optional<BlockMeta> &block_meta, BlockID block_id);

    static Status LoadFlushedBlock1(SegmentMeta &segment_meta, const WalBlockInfo &block_info, TxnTimeStamp checkpoint_ts);

    static Status CleanBlock(BlockMeta &block_meta, UsageFlag usage_flag);

    static Status
    AddNewBlockColumn(BlockMeta &block_meta, size_t column_idx, const std::shared_ptr<ColumnDef> &column_def, std::optional<ColumnMeta> &column_meta);

    static Status CleanBlockColumn(ColumnMeta &column_meta, const std::shared_ptr<ColumnDef> &column_def, UsageFlag usage_flag);

    static Status RestoreNewSegmentIndex1(TableIndexMeta &table_index_meta,
                                          NewTxn *new_txn,
                                          SegmentID segment_id,
                                          std::optional<SegmentIndexMeta> &segment_index_meta,
                                          ChunkID next_chunk_id);

    static Status
    AddNewSegmentIndex1(TableIndexMeta &table_index_meta, NewTxn *new_txn, SegmentID segment_id, std::optional<SegmentIndexMeta> &segment_index_meta);

    static Status CleanSegmentIndex(SegmentIndexMeta &segment_index_meta, UsageFlag usage_flag);

    static Status AddNewChunkIndex1(SegmentIndexMeta &segment_index_meta,
                                    NewTxn *new_txn,
                                    ChunkID chunk_id,
                                    RowID base_row_id,
                                    size_t row_count,
                                    size_t term_count,
                                    const std::string &base_name,
                                    size_t index_size,
                                    std::optional<ChunkIndexMeta> &chunk_index_meta);

    static Status RestoreNewChunkIndex1(SegmentIndexMeta &segment_index_meta,
                                        NewTxn *new_txn,
                                        ChunkID chunk_id,
                                        RowID base_row_id,
                                        size_t row_count,
                                        size_t term_count,
                                        const std::string &base_name,
                                        size_t index_size,
                                        std::optional<ChunkIndexMeta> &chunk_index_meta,
                                        bool is_link_files = false);

    static Status LoadFlushedChunkIndex1(SegmentIndexMeta &segment_index_meta, const WalChunkIndexInfo &chunk_info, NewTxn *new_txn);

    static Status CleanChunkIndex(ChunkIndexMeta &chunk_index_meta, UsageFlag usage_flag);

    static Status GetColumnVector(ColumnMeta &column_meta,
                                  const std::shared_ptr<ColumnDef> &col_def,
                                  size_t row_count,
                                  const ColumnVectorMode &tipe,
                                  ColumnVector &column_vector);

    static Status GetBlockVisibleRange(BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, NewTxnGetVisibleRangeState &state);

    static Status GetCreateTSVector(BlockMeta &block_meta, size_t offset, size_t row_count, ColumnVector &column_vector);

    static Status GetDeleteTSVector(BlockMeta &block_meta, size_t offset, size_t row_count, ColumnVector &column_vector);

    static Status GetDBFilePaths(TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, DBMeta &db_meta, std::vector<std::string> &file_paths);

    static Status GetTableFilePaths(TxnTimeStamp begin_ts,
                                    TableMeta &table_meta,
                                    std::vector<std::string> &file_paths,
                                    std::shared_ptr<ColumnDef> column_def = nullptr);

    static Status GetSegmentFilePaths(TxnTimeStamp begin_ts,
                                      SegmentMeta &segment_meta,
                                      std::vector<std::string> &file_paths,
                                      std::shared_ptr<ColumnDef> column_def = nullptr);

    static Status GetBlockFilePaths(BlockMeta &block_meta, std::vector<std::string> &file_paths, std::shared_ptr<ColumnDef> column_def = nullptr);

    static Status GetBlockColumnFilePaths(ColumnMeta &column_meta, std::vector<std::string> &file_paths);

    static Status
    GetColumnFilePaths(TxnTimeStamp begin_ts, TableMeta &table_meta, std::shared_ptr<ColumnDef> column_def, std::vector<std::string> &file_paths);

    static Status GetTableIndexFilePaths(TableIndexMeta &table_index_meta, std::vector<std::string> &file_paths);

    static Status GetSegmentIndexFilepaths(SegmentIndexMeta &segment_index_meta, std::vector<std::string> &file_paths);

    static Status GetChunkIndexFilePaths(ChunkIndexMeta &chunk_index_meta, std::vector<std::string> &file_paths);

    static Status CheckColumnIfIndexed(TableMeta &table_meta, ColumnID column_id, bool &has_index);

    static Status CheckTableIfDelete(TableMeta &table_meta, TxnTimeStamp begin_ts, bool &has_delete);

    static Status SetBlockDeleteBitmask(BlockMeta &block_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, Bitmask &bitmask);

    static Status CheckSegmentRowsVisible(SegmentMeta &segment_meta, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, Bitmask &bitmask);

public:
    // Function related methods
    static std::shared_ptr<FunctionSet> GetFunctionSetByName(NewCatalog *catalog, std::string function_name);

    static void AddFunctionSet(NewCatalog *catalog, const std::shared_ptr<FunctionSet> &function_set);

    static void AppendToScalarFunctionSet(NewCatalog *catalog, const std::shared_ptr<FunctionSet> &function_set);

    static void AddSpecialFunction(NewCatalog *catalog, const std::shared_ptr<SpecialFunction> &special_function);

    static std::tuple<SpecialFunction *, Status> GetSpecialFunctionByNameNoExcept(NewCatalog *catalog, std::string function_name);
};
} // namespace infinity
