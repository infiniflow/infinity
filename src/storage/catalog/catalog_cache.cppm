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

export module catalog_cache;

import stl;
// import status;
// import meta_info;
// import extra_ddl_info;
import default_values;
import internal_types;
import third_party;
import status;
// import buffer_handle;
// import column_def;
// import profiler;
// import storage;
// import catalog_delta_entry;

// using SegmentID = uint32_t;
// using ChunkID = uint32_t;
// using BlockID = uint16_t;
// using ColumnID = uint64_t;
//
//// Related to entry
// using BlockOffset = uint16_t;
// using SegmentOffset = uint32_t;

namespace infinity {

class IndexBase;

export struct SegmentIndexPrepareInfo {
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    SizeT row_count_{};
};

export struct CreateIndexPrepareInfo {
    u64 index_id_{};
    String index_name_{};
    Vector<SegmentIndexPrepareInfo> segment_index_prepare_infos_;
};

export struct ImportPrepareInfo {
    Vector<SegmentID> segment_ids_;
    Vector<u64> row_counts_; // assigned by import operation in prepare commit phase. row count of each imported segment.
    Vector<CreateIndexPrepareInfo> indexes_;
};

export struct CompactPrepareInfo {
    SegmentID new_segment_id_;
    Vector<SegmentID> old_segment_ids_;
    Vector<CreateIndexPrepareInfo> indexes_;
};

export struct OptimizeSegmentIndexPrepareInfo {
    SegmentID new_segment_id_;
    Vector<ChunkID> old_chunk_ids_;
    ChunkID new_chunk_id_;
};

export struct OptimizePrepareInfo {
    u64 index_id_{};
    Vector<OptimizeSegmentIndexPrepareInfo> segment_index_prepare_infos_;
};

export struct AppendPrepareInfo {
    TransactionID transaction_id_;
    Vector<Pair<RowID, u64>> ranges_;
};

export struct SegmentIndexCache {
public:
    explicit SegmentIndexCache(SegmentID segment_id) : segment_id_(segment_id) {}

    SegmentID segment_id() const { return segment_id_; }
    ChunkID next_chunk_id() const { return next_chunk_id_; }

    SegmentID segment_id_{};
    ChunkID next_chunk_id_{};
    Map<ChunkID, Pair<RowID, u64>> chunk_row_ranges_{}; // For current segment
};

export struct TableIndexCache {
public:
    explicit TableIndexCache(u64 db_id, u64 table_id, u64 index_id, const String &index_name)
        : db_id_(db_id), table_id_(table_id), index_id_(index_id), index_name_(index_name) {}

    u64 db_id_{};
    u64 table_id_{};
    u64 index_id_{};
    String index_name_{};
    Map<SegmentID, SharedPtr<SegmentIndexCache>> segment_index_cache_map_{}; // segment_id -> segment_index_cache
};

export struct SegmentCache {
public:
    SegmentCache(SegmentID segment_id, u64 row_count) : segment_id_(segment_id), row_count_(row_count) {}

    SegmentID segment_id_{};
    u64 row_count_{};
    bool sealed_{false}; // true if the segment is sealed
};

export class TableCache {
public:
    // Used when the table is created
    explicit TableCache(u64 table_id, const String &table_name_)
        : table_id_(table_id), table_name_(table_name_) {}

    // Used when system is restarted.
    TableCache(u64 table_id, SegmentID unsealed_segment_id, SegmentOffset unsealed_segment_offset, SegmentID next_segment_id);

    // Getter
    u64 table_id() const { return table_id_; };

    SegmentID prepare_segment_id() const { return prepare_segment_id_; }
    SegmentOffset prepare_segment_offset() const { return prepare_segment_offset_; }
    SegmentID commit_segment_id() const { return commit_segment_id_; }
    SegmentOffset commit_segment_offset() const { return commit_segment_offset_; }

    SegmentID next_segment_id() const { return next_segment_id_; }
    u64 next_index_id() const { return next_index_id_; }

    SharedPtr<AppendPrepareInfo> PrepareAppendNolock(SizeT row_count, TransactionID txn_id);
    void CommitAppendNolock(const SharedPtr<AppendPrepareInfo> &append_info, TransactionID txn_id);
    bool AllPrepareAreCommittedNolock() const;
    RowID GetCommitPosition() const;
    SegmentID prepare_segment_id_{0};
    SegmentOffset prepare_segment_offset_{0};
    SegmentID commit_segment_id_{0};
    SegmentOffset commit_segment_offset_{0};

    // Import segments
    Tuple<SharedPtr<ImportPrepareInfo>, Status> PrepareImportSegmentsNolock(u64 segment_count, TransactionID txn_id);
    void CommitImportSegmentsNolock(const SharedPtr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id);

    // Compact segments
    Tuple<SharedPtr<CompactPrepareInfo>, Status> PrepareCompactSegmentsNolock(const Vector<SegmentID> &segment_ids);
    void CommitCompactSegmentsNolock(const SharedPtr<CompactPrepareInfo> &import_prepare_info);

    // Optimize segments
    Tuple<SharedPtr<OptimizePrepareInfo>, Status> PrepareOptimizeSegmentsNolock(const Vector<SegmentID> &segment_ids);
    Tuple<SharedPtr<OptimizePrepareInfo>, Status> PrepareOptimizeSegmentsNolock();
    void CommitOptimizeSegmentsNolock(const SharedPtr<OptimizePrepareInfo> &import_prepare_info);

    Tuple<SharedPtr<ImportPrepareInfo>, Status> GetNewSegmentIDsNolock(SizeT segment_count);
    Pair<RowID, u64> PrepareDumpIndexRangeNolock(u64 index_id); // used by dump mem index and create index

    void AddTableIndexCacheNolock(const SharedPtr<TableIndexCache> &table_index_cache);
    void DropTableIndexCacheNolock(u64 index_id);

    String ToString() const;

    u64 table_id_{};
    String table_name_{};

    // Used by append and import
    SegmentID next_segment_id_{0};

    // Used by create index
    u64 next_index_id_{0};

    // Used by append
    Deque<Tuple<RowID, u64, TransactionID>> prepared_append_ranges_{};

    Map<SegmentID, SharedPtr<SegmentCache>> segment_cache_map_{}; // segment_id -> segment_cache

    Map<SegmentID, SharedPtr<SegmentCache>> sealed_segment_cache_map_{}; // segment_id -> segment_cache
    SharedPtr<SegmentCache> unsealed_segment_cache_{};
    Deque<SharedPtr<AppendPrepareInfo>> uncommitted_append_infos_{};

    // Used by dump index / create index, index_id -> table_index_cache
    Map<u64, SharedPtr<TableIndexCache>> index_cache_map_{}; // index_id -> table_index_cache
    Map<String, u64> index_name_map_{};                      // index_name -> index_id

    Map<TransactionID, SharedPtr<ImportPrepareInfo>> import_prepare_info_map_{}; // txn_id -> import_prepare_info
};

export struct DbCache {
public:
    explicit DbCache(u64 db_id, const String &db_name, u64 next_table_id_) : db_id_(db_id), db_name_(db_name), next_table_id_(next_table_id_) {};

    void AddTableCacheNolock(const SharedPtr<TableCache> &table_cache);
    void AddNewTableCacheNolock(u64 table_id, const String &table_name);
    void DropTableCacheNolock(u64 table_id);

    u64 db_id() const { return db_id_; }
    const String &db_name() const { return db_name_; }

    u64 db_id_{};
    String db_name_{};
    u64 next_table_id_{};
    Map<u64, SharedPtr<TableCache>> table_cache_map_{};
};

export class SystemCache {
public:
    explicit SystemCache(u64 next_db_id) : next_db_id_(next_db_id) {}
    // Create db
    void AddNewDbCache(const String &db_name, u64 db_id);

    // Drop db
    void DropDbCache(u64 db_id);

    // Create table
    void AddNewTableCache(u64 db_id, u64 table_id, const String &table_name);

    // Drop table
    void DropTableCache(u64 db_id, u64 table_id);

    // Create index
    Tuple<u64, Status> AddNewIndexCache(u64 db_id, u64 table_id, const String &index_name);
    Tuple<SharedPtr<CreateIndexPrepareInfo>, Status> PrepareNewIndexCache(u64 db_id, u64 table_id, const String &index_name);
    void CommitNewIndexCache(u64 db_id, u64 table_id, const SharedPtr<CreateIndexPrepareInfo> &index_prepare_info);

    // Drop index
    void DropIndexCache(u64 db_id, u64 table_id, u64 index_id);

    // Import segments
    Tuple<SharedPtr<ImportPrepareInfo>, Status> PrepareImportSegments(u64 db_id, u64 table_id, u64 segment_count, TransactionID txn_id);
    void CommitImportSegments(u64 db_id, u64 table_id, const SharedPtr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id);

    // Compact segments
    Tuple<SharedPtr<CompactPrepareInfo>, Status> PrepareCompactSegments(u64 db_id, u64 table_id, const Vector<SegmentID> &segment_ids);
    void CommitCompactSegments(u64 db_id, u64 table_id, const SharedPtr<CompactPrepareInfo> &import_prepare_info);

    // Optimize segments
    Tuple<SharedPtr<OptimizePrepareInfo>, Status> PrepareOptimizeSegments(u64 db_id, u64 table_id, const Vector<SegmentID> &segment_ids);
    Tuple<SharedPtr<OptimizePrepareInfo>, Status> PrepareOptimizeSegments(u64 db_id, u64 table_id);
    void CommitOptimizeSegments(u64 db_id, u64 table_id, const SharedPtr<OptimizePrepareInfo> &import_prepare_info);

    // Append and update
    SharedPtr<AppendPrepareInfo> PrepareAppend(u64 db_id, u64 table_id, SizeT row_count, TransactionID txn_id);
    void CommitAppend(u64 db_id, u64 table_id, const SharedPtr<AppendPrepareInfo> &append_info, TransactionID txn_id);

    nlohmann::json ToJson() const;

    // Used by restore
    Status AddDbCacheNolock(const SharedPtr<DbCache> &db_cache);
    SharedPtr<DbCache> GetDbCache(u64 db_id) const;

private:
    DbCache *GetDbCacheNolock(u64 db_id);
    TableCache *GetTableCacheNolock(u64 db_id, u64 table_id);

    mutable std::mutex cache_mtx_{};
    u64 next_db_id_{};
    Map<u64, SharedPtr<DbCache>> db_cache_map_{}; // db_id -> db_cache
    Map<String, u64> db_name_map_{};              // db_name -> db_id
};

} // namespace infinity