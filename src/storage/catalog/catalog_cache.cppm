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

export class SegmentIndexCache {
public:
    explicit SegmentIndexCache(SegmentID segment_id) : segment_id_(segment_id) {}

    SegmentID segment_id() const { return segment_id_; }
    ChunkID next_chunk_id() const { return next_chunk_id_; }

private:
    SegmentID segment_id_{};
    ChunkID next_chunk_id_{};
    Map<ChunkID, Pair<RowID, u64>> chunk_row_ranges_{}; // For current segment
};

export class TableIndexCache {
public:
    explicit TableIndexCache(u64 db_id, u64 table_id, u64 index_id) : db_id_(db_id), table_id_(table_id), index_id_(index_id) {}

    u64 index_id() const { return index_id_; }

private:
    u64 db_id_{};
    u64 table_id_{};
    u64 index_id_{};

    // Used by optimize index and dump mem index
    ChunkID next_chunk_id_{0};

    // Used by dump mem index
    Vector<Pair<RowID, u64>> chunk_row_ranges_{}; // For current segment
};

export class TableCache {
public:
    // Used when the table is created
    explicit TableCache(u64 db_id, u64 table_id)
        : db_id_(db_id), table_id_(table_id), all_segments_sealed_prepare_(true), all_segments_sealed_commit_(true) {}

    // Used when system is restarted.
    TableCache(u64 db_id, u64 table_id, SegmentID unsealed_segment_id, SegmentOffset unsealed_segment_offset, SegmentID next_segment_id);

    // Getter
    bool has_prepared_unsealed_segment() const { return all_segments_sealed_prepare_; }
    bool has_commit_unsealed_segment() const { return all_segments_sealed_commit_; }
    u64 table_id() const { return table_id_; };
    SegmentID prepare_unsealed_segment_id() const { return prepare_unsealed_segment_id_; }
    SegmentOffset prepare_unsealed_segment_offset() const { return prepare_unsealed_segment_offset_; }
    SegmentID commit_unsealed_segment_id() const { return commit_unsealed_segment_id_; }
    SegmentOffset commit_unsealed_segment_offset() const { return commit_unsealed_segment_offset_; }

    SegmentID next_segment_id() const { return next_segment_id_; }
    u64 next_index_id() const { return next_index_id_; }

    Vector<Pair<RowID, u64>> PrepareAppendRanges(SizeT row_count, TransactionID txn_id);
    void CommitAppendRanges(const Vector<Pair<RowID, u64>> &ranges, TransactionID txn_id);
    RowID GetCommitUnsealedPosition();

    Vector<SegmentID> GetImportSegments(SizeT segment_count);
    SegmentID GetCompactSegment();
    Pair<RowID, u64> PrepareDumpIndexRange(u64 index_id); // used by dump mem index and create index

    void AddTableIndexCache(const SharedPtr<TableIndexCache> &table_index_cache);
    void DropTableIndexCache(u64 index_id);

    String ToString() const;

private:
    u64 db_id_{};
    u64 table_id_{};

    bool all_segments_sealed_prepare_{false};
    bool all_segments_sealed_commit_{false};

    // Used by append
    SegmentID prepare_unsealed_segment_id_{0};
    SegmentOffset prepare_unsealed_segment_offset_{0};
    SegmentID commit_unsealed_segment_id_{0};
    SegmentOffset commit_unsealed_segment_offset_{0};

    // Used by append and import
    SegmentID next_segment_id_{0};

    // Used by create index
    u64 next_index_id_{0};

    // Used by append
    Deque<Tuple<RowID, u64, TransactionID>> prepared_append_ranges_{};

    // Used by dump index / create index
    Map<u64, SharedPtr<TableIndexCache>> index_cache_map_{};
};

export class DbCache {
public:
    explicit DbCache(u64 db_id, const String &db_name, u64 next_table_id_) : db_id_(db_id), db_name_(db_name), next_table_id_(next_table_id_) {};
    u64 AddNewTableCache();

    void AddTableCache(const SharedPtr<TableCache> &table_cache);
    void DropTableCache(u64 table_id);

    u64 db_id() const { return db_id_; }
    const String &db_name() const { return db_name_; }

private:
    u64 db_id_{};
    String db_name_{};
    u64 next_table_id_{};
    Map<u64, SharedPtr<TableCache>> table_cache_map_{};
};

export class SystemCache {
public:
    explicit SystemCache(u64 next_db_id) : next_db_id_(next_db_id) {}
    void AddNewDbCache(const String &db_name, u64 db_id);
    u64 AddNewTableCache(u64 db_id);
    nlohmann::json ToJson() const;

    // Used by restore
    Status AddDbCacheNolock(const SharedPtr<DbCache> &db_cache);
    SharedPtr<DbCache> GetDbCache(u64 db_id) const;
    void DropDbCache(u64 db_id);

private:
    mutable std::mutex cache_mtx_{};
    u64 next_db_id_{};
    Map<u64, SharedPtr<DbCache>> db_cache_map_{};
    Map<String, u64> db_name_map_{};
};

} // namespace infinity