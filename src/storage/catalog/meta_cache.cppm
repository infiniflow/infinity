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

export module infinity_core:meta_cache;

import :stl;
import :default_values;
import internal_types;
import :third_party;
import :status;

namespace infinity {

class KVInstance;

export enum class MetaCacheType {
    kInvalid,
    kCreateDB,
    kCreateTable,
    kCreateIndex,
};

export enum class EraseCacheType {
    kInvalid,
    kEraseDB,
    kEraseTable,
    kEraseIndex,
};

struct MetaBaseCache {
    explicit MetaBaseCache(MetaCacheType type) : type_(type) {}
    virtual ~MetaBaseCache() = default;
    MetaCacheType type_{MetaCacheType::kInvalid};

    virtual String name() = 0;
    virtual u64 commit_ts() = 0;
    virtual String type() = 0;
    virtual String detail() = 0;
};

export struct MetaDbCache final : public MetaBaseCache {
    MetaDbCache(const String &db_name, u64 db_id, u64 commit_ts, const String &db_key, bool is_dropped)
        : MetaBaseCache(MetaCacheType::kCreateDB), db_name_(db_name), db_id_(db_id), commit_ts_(commit_ts), db_key_(db_key), is_dropped_(is_dropped) {
    }
    String db_name_{};
    u64 db_id_{};
    u64 commit_ts_{};
    String db_key_{};
    bool is_dropped_{false};

    bool get_comment_{false};
    SharedPtr<String> comment_{};

    bool get_tables_{false};
    Vector<String> table_ids_{};
    Vector<String> table_names_{};

    String name() final;
    u64 commit_ts() final;
    String type() final;
    String detail() final;
};

export struct MetaTableCache : public MetaBaseCache {
    MetaTableCache(u64 db_id, const String &table_name, u64 table_id, u64 commit_ts, const String &table_key, bool is_dropped)
        : MetaBaseCache(MetaCacheType::kCreateTable), db_id_(db_id), table_name_(table_name), table_id_(table_id), commit_ts_(commit_ts),
          table_key_(table_key), is_dropped_(is_dropped) {}
    u64 db_id_{};
    String table_name_{};
    u64 table_id_{};
    u64 commit_ts_{};
    String table_key_{};
    bool is_dropped_{false};

    String name() final;
    u64 commit_ts() final;
    String type() final;
    String detail() final;
};

export struct MetaIndexCache : public MetaBaseCache {
    MetaIndexCache(u64 db_id, u64 table_id, const String &index_name, u64 index_id, u64 commit_ts, const String &index_key, bool is_dropped)
        : MetaBaseCache(MetaCacheType::kCreateIndex), db_id_(db_id), table_id_(table_id), index_name_(index_name), index_id_(index_id),
          index_key_(index_key), commit_ts_(commit_ts), is_dropped_(is_dropped) {};
    u64 db_id_{};
    u64 table_id_{};
    String index_name_{};
    u64 index_id_{};
    String index_key_{};
    u64 commit_ts_{};
    bool is_dropped_{false};

    String name() final;
    u64 commit_ts() final;
    String type() final;
    String detail() final;
};

export struct EraseBaseCache {
    explicit EraseBaseCache(EraseCacheType type) : type_(type) {}
    virtual ~EraseBaseCache() = default;
    EraseCacheType type_{EraseCacheType::kInvalid};
};

export struct MetaEraseDbCache : public EraseBaseCache {
    explicit MetaEraseDbCache(const String &db_name) : EraseBaseCache(EraseCacheType::kEraseDB), db_name_(db_name) {}
    String db_name_{};
};

export struct MetaEraseTableCache : public EraseBaseCache {
    explicit MetaEraseTableCache(u64 db_id, const String &table_name)
        : EraseBaseCache(EraseCacheType::kEraseTable), db_id_(db_id), table_name_(table_name) {}
    u64 db_id_{};
    String table_name_{};
};

export struct MetaEraseIndexCache : public EraseBaseCache {
    explicit MetaEraseIndexCache(u64 db_id, u64 table_id, const String &index_name)
        : EraseBaseCache(EraseCacheType::kEraseIndex), db_id_(db_id), table_id_(table_id), index_name_(index_name) {}
    u64 db_id_{};
    u64 table_id_{};
    String index_name_{};
};

struct CacheItem {
    String name_;
    SharedPtr<MetaBaseCache> meta_cache_;
};

struct TableNameID {
    String name_{};
    TxnTimeStamp commit_ts_{};
    MetaCacheType meta_type_{MetaCacheType::kInvalid};
};

export class MetaCache {
    mutable std::mutex cache_mtx_{};
    HashMap<String, Map<u64, List<CacheItem>::iterator>> dbs_;     // db_name -> (commit_ts -> MetaDbCache)
    HashMap<String, Map<u64, List<CacheItem>::iterator>> tables_;  // table_name -> (commit_ts -> MetaTableCache)
    HashMap<String, Map<u64, List<CacheItem>::iterator>> indexes_; // index_name -> (commit_ts -> MetaIndexCache)

    SizeT capacity_{0};
    List<CacheItem> lru_{};

public:
    explicit MetaCache(SizeT capacity) : capacity_(capacity) {};

    void Put(const Vector<SharedPtr<MetaBaseCache>> &cache_items);

    Status Erase(const Vector<SharedPtr<EraseBaseCache>> &cache_items, KVInstance *kv_instance);

    Status PutOrErase(const Vector<SharedPtr<MetaBaseCache>> &cache_items, KVInstance *kv_instance);

    SharedPtr<MetaDbCache> GetDb(const String &db_name, TxnTimeStamp begin_ts);

    SharedPtr<MetaTableCache> GetTable(u64 db_id, const String &table_name, TxnTimeStamp begin_ts);

    SharedPtr<MetaIndexCache> GetIndex(u64 db_id, u64 table_id, const String &index_name, TxnTimeStamp begin_ts);

    void PrintLRU() const;

    SizeT Size() const;

    Vector<SharedPtr<MetaBaseCache>> GetAllCacheItems() const;

private:
    void PutOrEraseNolock(const SharedPtr<MetaBaseCache> &meta_base_cache);
    void PutDbNolock(const SharedPtr<MetaDbCache> &db_cache);
    void EraseDbNolock(const String &db_name);
    void PutTableNolock(const SharedPtr<MetaTableCache> &table_cache);
    void EraseTableNolock(u64 db_id, const String &table_name);
    void PutIndexNolock(const SharedPtr<MetaIndexCache> &index_cache);
    void EraseIndexNolock(u64 db_id, u64 table_id, const String &index_name);
    void TrimCacheNolock();
    void TouchNolock(List<CacheItem>::iterator iter);
};

// class IndexBase;
//
// export struct SegmentIndexPrepareInfo {
//     SegmentID segment_id_{};
//     ChunkID chunk_id_{};
//     SizeT row_count_{};
// };
//
// export struct DumpMemIndexPrepareInfo {
//     SegmentID segment_id_{};
//     ChunkID chunk_id_{};
//     SegmentOffset start_offset_{};
//     SizeT row_count_{};
//     u64 index_id_;
// };
//
// export struct CreateIndexPrepareInfo {
//     u64 index_id_{};
//     String index_name_{};
//     Vector<SegmentIndexPrepareInfo> segment_index_prepare_infos_;
// };
//
// export struct ImportPrepareInfo {
//     Vector<SegmentID> segment_ids_;
//     Vector<u64> row_counts_; // assigned by import operation in prepare commit phase. row count of each imported segment.
//     Vector<CreateIndexPrepareInfo> indexes_;
// };
//
// export struct CompactPrepareInfo {
//     SegmentID new_segment_id_{};
//     u64 new_segment_row_count_{};
//     Vector<SegmentID> old_segment_ids_{};
//     Vector<CreateIndexPrepareInfo> indexes_{};
// };
//
// export struct OptimizeSegmentIndexPrepareInfo {
//     SegmentID new_segment_id_;
//     Vector<ChunkID> old_chunk_ids_;
//     ChunkID new_chunk_id_;
// };
//
// export struct OptimizePrepareInfo {
//     u64 index_id_{};
//     Vector<OptimizeSegmentIndexPrepareInfo> segment_index_prepare_infos_;
// };
//
// export struct AppendPrepareInfo {
//     TransactionID transaction_id_;
//     Vector<Pair<RowID, u64>> ranges_;
// };
//
// export struct SegmentIndexCache {
// public:
//     explicit SegmentIndexCache(SegmentID segment_id) : segment_id_(segment_id) {}
//
//     SegmentID segment_id() const { return segment_id_; }
//     ChunkID next_chunk_id() const { return next_chunk_id_; }
//
//     SegmentID segment_id_{};
//     ChunkID next_chunk_id_{};
//     //    Map<ChunkID, Pair<RowID, u64>> chunk_row_ranges_{}; // For current segment
// };
//
// export struct TableIndexCache {
// public:
//     explicit TableIndexCache(u64 db_id, u64 table_id, u64 index_id, const String &index_name)
//         : db_id_(db_id), table_id_(table_id), index_id_(index_id), index_name_(index_name) {}
//
//     u64 db_id_{};
//     u64 table_id_{};
//     u64 index_id_{};
//     String index_name_{};
//     Map<SegmentID, SharedPtr<SegmentIndexCache>> segment_index_cache_map_{}; // segment_id -> segment_index_cache
// };
//
// export struct SegmentCache {
// public:
//     SegmentCache(SegmentID segment_id, u64 row_count) : segment_id_(segment_id), row_count_(row_count) {}
//
//     SegmentID segment_id_{};
//     u64 row_count_{};
//     bool sealed_{false}; // true if the segment is sealed
// };
//
// export class TableCache {
// public:
//     // Used when the table is created
//     explicit TableCache(u64 table_id, const String &table_name_) : table_id_(table_id), table_name_(table_name_) {}
//
//     // Used when system is restarted.
//     TableCache(u64 table_id, SegmentID unsealed_segment_id, SegmentOffset unsealed_segment_offset, SegmentID next_segment_id);
//
//     // Getter
//     u64 table_id() const { return table_id_; };
//
//     SegmentID prepare_segment_id() const { return prepare_segment_id_; }
//     SegmentOffset prepare_segment_offset() const { return prepare_segment_offset_; }
//     SegmentID commit_segment_id() const { return commit_segment_id_; }
//     SegmentOffset commit_segment_offset() const { return commit_segment_offset_; }
//
//     SegmentID next_segment_id() const { return next_segment_id_; }
//     u64 next_index_id() const { return next_index_id_; }
//
//     SharedPtr<AppendPrepareInfo> PrepareAppendNolock(SizeT row_count, TransactionID txn_id);
//     void CommitAppendNolock(const SharedPtr<AppendPrepareInfo> &append_info, TransactionID txn_id);
//     bool AllPrepareAreCommittedNolock() const;
//     RowID GetCommitPosition() const;
//     SegmentID prepare_segment_id_{0};
//     SegmentOffset prepare_segment_offset_{0};
//     SegmentID commit_segment_id_{0};
//     SegmentOffset commit_segment_offset_{0};
//
//     // Import segments
//     SharedPtr<ImportPrepareInfo> PrepareImportSegmentsNolock(u64 segment_count, TransactionID txn_id);
//     void CommitImportSegmentsNolock(const SharedPtr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id);
//
//     // Compact segments
//     SharedPtr<CompactPrepareInfo> PrepareCompactSegmentsNolock(const Vector<SegmentID> &segment_ids, TransactionID txn_id);
//     void CommitCompactSegmentsNolock(const SharedPtr<CompactPrepareInfo> &compact_prepare_info, TransactionID txn_id);
//
//     Vector<SegmentID> ApplySegmentIDsNolock(u64 segment_count);
//     ChunkID ApplyChunkIDNolock(u64 index_id, SegmentID segment_id);
//
//     void AddTableIndexCacheNolock(const SharedPtr<TableIndexCache> &table_index_cache);
//     void DropTableIndexCacheNolock(u64 index_id);
//
//     String ToString() const;
//
//     u64 table_id_{};
//     String table_name_{};
//
//     // Used by append and import
//     SegmentID next_segment_id_{0};
//
//     // Used by create index
//     u64 next_index_id_{0};
//
//     // Used by append
//     Deque<Tuple<RowID, u64, TransactionID>> prepared_append_ranges_{};
//
//     Map<SegmentID, SharedPtr<SegmentCache>> segment_cache_map_{}; // segment_id -> segment_cache
//
//     Map<SegmentID, SharedPtr<SegmentCache>> sealed_segment_cache_map_{}; // segment_id -> segment_cache
//     SharedPtr<SegmentCache> unsealed_segment_cache_{};
//     Deque<SharedPtr<AppendPrepareInfo>> uncommitted_append_infos_{};
//
//     // Used by dump index / create index, index_id -> table_index_cache
//     Map<u64, SharedPtr<TableIndexCache>> index_cache_map_{}; // index_id -> table_index_cache
//     Map<String, u64> index_name_map_{};                      // index_name -> index_id
//
//     Map<TransactionID, SharedPtr<ImportPrepareInfo>> import_prepare_info_map_{};   // txn_id -> import_prepare_info
//     Map<TransactionID, SharedPtr<CompactPrepareInfo>> compact_prepare_info_map_{}; // txn_id -> import_prepare_info
// };
//
// export struct DbCache {
// public:
//     explicit DbCache(u64 db_id, const String &db_name, u64 next_table_id_) : db_id_(db_id), db_name_(db_name), next_table_id_(next_table_id_) {};
//
//     void AddTableCacheNolock(const SharedPtr<TableCache> &table_cache);
//     void AddNewTableCacheNolock(u64 table_id, const String &table_name);
//     void DropTableCacheNolock(u64 table_id);
//
//     u64 db_id() const { return db_id_; }
//     const String &db_name() const { return db_name_; }
//
//     u64 db_id_{};
//     String db_name_{};
//     u64 next_table_id_{};
//     Map<u64, SharedPtr<TableCache>> table_cache_map_{};
// };
//
// export class SystemCache {
// public:
//     explicit SystemCache(u64 next_db_id) : next_db_id_(next_db_id) {}
//     // Create db
//     void AddNewDbCache(const String &db_name, u64 db_id);
//
//     // Drop db
//     void DropDbCache(u64 db_id);
//
//     // Create table
//     void AddNewTableCache(u64 db_id, u64 table_id, const String &table_name);
//
//     // Drop table
//     void DropTableCache(u64 db_id, u64 table_id);
//
//     // Create index to cache
//     void AddNewIndexCache(u64 db_id, u64 table_id, const String &index_name);
//
//     // Drop index
//     void DropIndexCache(u64 db_id, u64 table_id, u64 index_id);
//
//     // Import segments
//     SharedPtr<ImportPrepareInfo> PrepareImportSegments(u64 db_id, u64 table_id, u64 segment_count, TransactionID txn_id);
//     void CommitImportSegments(u64 db_id, u64 table_id, const SharedPtr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id);
//
//     // Compact segments
//     SharedPtr<CompactPrepareInfo> PrepareCompactSegments(u64 db_id, u64 table_id, const Vector<SegmentID> &segment_ids, TransactionID txn_id);
//     void CommitCompactSegments(u64 db_id, u64 table_id, const SharedPtr<CompactPrepareInfo> &compact_prepare_info, TransactionID txn_id);
//
//     Vector<SegmentID> ApplySegmentIDs(u64 db_id, u64 table_id, u64 segment_count);
//     ChunkID ApplyChunkID(u64 db_id, u64 table_id, u64 index_id, SegmentID segment_id);
//
//     // Append and update
//     SharedPtr<AppendPrepareInfo> PrepareAppend(u64 db_id, u64 table_id, SizeT row_count, TransactionID txn_id);
//     void CommitAppend(u64 db_id, u64 table_id, const SharedPtr<AppendPrepareInfo> &append_info, TransactionID txn_id);
//
//     nlohmann::json ToJson() const;
//
//     // Used by restore
//     Status AddDbCacheNolock(const SharedPtr<DbCache> &db_cache);
//     SharedPtr<DbCache> GetDbCache(u64 db_id) const;
//
// private:
//     DbCache *GetDbCacheNolock(u64 db_id);
//     TableCache *GetTableCacheNolock(u64 db_id, u64 table_id);
//
//     mutable std::mutex cache_mtx_{};
//     u64 next_db_id_{};
//     Map<u64, SharedPtr<DbCache>> db_cache_map_{}; // db_id -> db_cache
//     Map<String, u64> db_name_map_{};              // db_name -> db_id
// };

} // namespace infinity