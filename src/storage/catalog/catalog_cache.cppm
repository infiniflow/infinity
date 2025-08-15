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

export module infinity_core:catalog_cache;

import :default_values;
import :status;

import third_party;

import internal_types;

namespace infinity {

class IndexBase;

export struct SegmentIndexPrepareInfo {
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    size_t row_count_{};
};

export struct DumpMemIndexPrepareInfo {
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
    SegmentOffset start_offset_{};
    size_t row_count_{};
    u64 index_id_;
};

export struct CreateIndexPrepareInfo {
    u64 index_id_{};
    std::string index_name_{};
    std::vector<SegmentIndexPrepareInfo> segment_index_prepare_infos_;
};

export struct ImportPrepareInfo {
    std::vector<SegmentID> segment_ids_;
    std::vector<u64> row_counts_; // assigned by import operation in prepare commit phase. row count of each imported segment.
    std::vector<CreateIndexPrepareInfo> indexes_;
};

export struct CompactPrepareInfo {
    SegmentID new_segment_id_{};
    u64 new_segment_row_count_{};
    std::vector<SegmentID> old_segment_ids_{};
    std::vector<CreateIndexPrepareInfo> indexes_{};
};

export struct OptimizeSegmentIndexPrepareInfo {
    SegmentID new_segment_id_;
    std::vector<ChunkID> old_chunk_ids_;
    ChunkID new_chunk_id_;
};

export struct OptimizePrepareInfo {
    u64 index_id_{};
    std::vector<OptimizeSegmentIndexPrepareInfo> segment_index_prepare_infos_;
};

export struct AppendPrepareInfo {
    TransactionID transaction_id_;
    std::vector<std::pair<RowID, u64>> ranges_;
};

export struct SegmentIndexCache {
public:
    explicit SegmentIndexCache(SegmentID segment_id) : segment_id_(segment_id) {}

    SegmentID segment_id() const { return segment_id_; }
    ChunkID next_chunk_id() const { return next_chunk_id_; }

    SegmentID segment_id_{};
    ChunkID next_chunk_id_{};
    //    std::map<ChunkID, std::pair<RowID, u64>> chunk_row_ranges_{}; // For current segment
};

export struct TableIndexCache {
public:
    explicit TableIndexCache(u64 db_id, u64 table_id, u64 index_id, const std::string &index_name)
        : db_id_(db_id), table_id_(table_id), index_id_(index_id), index_name_(index_name) {}

    u64 db_id_{};
    u64 table_id_{};
    u64 index_id_{};
    std::string index_name_{};
    std::map<SegmentID, std::shared_ptr<SegmentIndexCache>> segment_index_cache_map_{}; // segment_id -> segment_index_cache
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
    explicit TableCache(u64 table_id, const std::string &table_name_) : table_id_(table_id), table_name_(table_name_) {}

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

    std::shared_ptr<AppendPrepareInfo> PrepareAppendNolock(size_t row_count, TransactionID txn_id);
    void CommitAppendNolock(const std::shared_ptr<AppendPrepareInfo> &append_info, TransactionID txn_id);
    bool AllPrepareAreCommittedNolock() const;
    RowID GetCommitPosition() const;
    SegmentID prepare_segment_id_{0};
    SegmentOffset prepare_segment_offset_{0};
    SegmentID commit_segment_id_{0};
    SegmentOffset commit_segment_offset_{0};

    // Import segments
    std::shared_ptr<ImportPrepareInfo> PrepareImportSegmentsNolock(u64 segment_count, TransactionID txn_id);
    void CommitImportSegmentsNolock(const std::shared_ptr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id);

    // Compact segments
    std::shared_ptr<CompactPrepareInfo> PrepareCompactSegmentsNolock(const std::vector<SegmentID> &segment_ids, TransactionID txn_id);
    void CommitCompactSegmentsNolock(const std::shared_ptr<CompactPrepareInfo> &compact_prepare_info, TransactionID txn_id);

    std::vector<SegmentID> ApplySegmentIDsNolock(u64 segment_count);
    ChunkID ApplyChunkIDNolock(u64 index_id, SegmentID segment_id);

    void AddTableIndexCacheNolock(const std::shared_ptr<TableIndexCache> &table_index_cache);
    void DropTableIndexCacheNolock(u64 index_id);

    std::string ToString() const;

    u64 table_id_{};
    std::string table_name_{};

    // Used by append and import
    SegmentID next_segment_id_{0};

    // Used by create index
    u64 next_index_id_{0};

    // Used by append
    std::deque<std::tuple<RowID, u64, TransactionID>> prepared_append_ranges_{};

    std::map<SegmentID, std::shared_ptr<SegmentCache>> segment_cache_map_{}; // segment_id -> segment_cache

    std::map<SegmentID, std::shared_ptr<SegmentCache>> sealed_segment_cache_map_{}; // segment_id -> segment_cache
    std::shared_ptr<SegmentCache> unsealed_segment_cache_{};
    std::deque<std::shared_ptr<AppendPrepareInfo>> uncommitted_append_infos_{};

    // Used by dump index / create index, index_id -> table_index_cache
    std::map<u64, std::shared_ptr<TableIndexCache>> index_cache_map_{}; // index_id -> table_index_cache
    std::map<std::string, u64> index_name_map_{};                      // index_name -> index_id

    std::map<TransactionID, std::shared_ptr<ImportPrepareInfo>> import_prepare_info_map_{};   // txn_id -> import_prepare_info
    std::map<TransactionID, std::shared_ptr<CompactPrepareInfo>> compact_prepare_info_map_{}; // txn_id -> import_prepare_info
};

export struct DbCache {
public:
    explicit DbCache(u64 db_id, const std::string &db_name, u64 next_table_id_) : db_id_(db_id), db_name_(db_name), next_table_id_(next_table_id_) {};

    void AddTableCacheNolock(const std::shared_ptr<TableCache> &table_cache);
    void AddNewTableCacheNolock(u64 table_id, const std::string &table_name);
    void DropTableCacheNolock(u64 table_id);

    u64 db_id() const { return db_id_; }
    const std::string &db_name() const { return db_name_; }

    u64 db_id_{};
    std::string db_name_{};
    u64 next_table_id_{};
    std::map<u64, std::shared_ptr<TableCache>> table_cache_map_{};
};

export class SystemCache {
public:
    explicit SystemCache(u64 next_db_id) : next_db_id_(next_db_id) {}
    // Create db
    void AddNewDbCache(const std::string &db_name, u64 db_id);

    // Drop db
    void DropDbCache(u64 db_id);

    // Create table
    void AddNewTableCache(u64 db_id, u64 table_id, const std::string &table_name);

    // Drop table
    void DropTableCache(u64 db_id, u64 table_id);

    // Create index to cache
    void AddNewIndexCache(u64 db_id, u64 table_id, const std::string &index_name);

    // Drop index
    void DropIndexCache(u64 db_id, u64 table_id, u64 index_id);

    // Import segments
    std::shared_ptr<ImportPrepareInfo> PrepareImportSegments(u64 db_id, u64 table_id, u64 segment_count, TransactionID txn_id);
    void CommitImportSegments(u64 db_id, u64 table_id, const std::shared_ptr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id);

    // Compact segments
    std::shared_ptr<CompactPrepareInfo> PrepareCompactSegments(u64 db_id, u64 table_id, const std::vector<SegmentID> &segment_ids, TransactionID txn_id);
    void CommitCompactSegments(u64 db_id, u64 table_id, const std::shared_ptr<CompactPrepareInfo> &compact_prepare_info, TransactionID txn_id);

    std::vector<SegmentID> ApplySegmentIDs(u64 db_id, u64 table_id, u64 segment_count);
    ChunkID ApplyChunkID(u64 db_id, u64 table_id, u64 index_id, SegmentID segment_id);

    // Append and update
    std::shared_ptr<AppendPrepareInfo> PrepareAppend(u64 db_id, u64 table_id, size_t row_count, TransactionID txn_id);
    void CommitAppend(u64 db_id, u64 table_id, const std::shared_ptr<AppendPrepareInfo> &append_info, TransactionID txn_id);

    nlohmann::json ToJson() const;

    // Used by restore
    Status AddDbCacheNolock(const std::shared_ptr<DbCache> &db_cache);
    std::shared_ptr<DbCache> GetDbCache(u64 db_id) const;

private:
    DbCache *GetDbCacheNolock(u64 db_id);
    TableCache *GetTableCacheNolock(u64 db_id, u64 table_id);

    mutable std::mutex cache_mtx_{};
    u64 next_db_id_{};
    std::map<u64, std::shared_ptr<DbCache>> db_cache_map_{}; // db_id -> db_cache
};

} // namespace infinity