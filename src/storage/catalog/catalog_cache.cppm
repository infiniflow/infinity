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
// import buffer_handle;
// import column_def;
// import profiler;
// import third_party;
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

export class TableIndexCache {
public:
    TableIndexCache(u64 index_id) : index_id_(index_id) {}

private:
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
        : db_id_(db_id), table_id_(table_id), has_prepare_unsealed_segment_(true), has_commit_unsealed_segment_(true) {}

    // Used when system is restarted.
    explicit TableCache(u64 db_id, u64 table_id, SegmentID unsealed_segment_id, SegmentOffset unsealed_segment_offset, SegmentID next_segment_id)
        : db_id_(db_id), table_id_(table_id), prepare_unsealed_segment_id_(unsealed_segment_id),
          prepare_unsealed_segment_offset_(unsealed_segment_offset), commit_unsealed_segment_id_(unsealed_segment_id),
          commit_unsealed_segment_offset_(unsealed_segment_offset), next_segment_id_(next_segment_id) {
        if (unsealed_segment_offset == DEFAULT_SEGMENT_CAPACITY) {
            has_prepare_unsealed_segment_ = true;
            has_commit_unsealed_segment_ = true;
        } else {
            has_prepare_unsealed_segment_ = true;
            has_commit_unsealed_segment_ = true;
        }
    }

    // Getter
    bool has_prepared_unsealed_segment() const { return has_prepare_unsealed_segment_; }
    bool has_commit_unsealed_segment() const { return has_commit_unsealed_segment_; }
    u64 table_id() const { return table_id_; };
    SegmentID prepare_unsealed_segment_id() const { return prepare_unsealed_segment_id_; }
    SegmentOffset prepare_unsealed_segment_offset() const { return prepare_unsealed_segment_offset_; }
    SegmentID commit_unsealed_segment_id() const { return commit_unsealed_segment_id_; }
    SegmentOffset commit_unsealed_segment_offset() const { return commit_unsealed_segment_offset_; }

    SegmentID next_segment_id() const { return next_segment_id_; }

    Vector<Pair<RowID, u64>> PrepareAppendRanges(SizeT row_count, TransactionID txn_id);
    void CommitAppendRanges(const Vector<Pair<RowID, u64>> &ranges, TransactionID txn_id);
    RowID GetCommitUnsealedPosition();

    Vector<SegmentID> GetImportSegments(SizeT segment_count);
    SegmentID GetCompactSegment();
    Pair<RowID, u64> PrepareDumpIndexRange(u64 index_id); // used by dump mem index and create index

    String ToString() const;

private:
    u64 db_id_{};
    u64 table_id_{};

    bool has_prepare_unsealed_segment_{false};
    bool has_commit_unsealed_segment_{false};

    // Used by append
    SegmentID prepare_unsealed_segment_id_{0};
    SegmentOffset prepare_unsealed_segment_offset_{0};
    SegmentID commit_unsealed_segment_id_{0};
    SegmentOffset commit_unsealed_segment_offset_{0};

    // Used by append and import
    SegmentID next_segment_id_{0};

    // Used by append
    Deque<Tuple<RowID, u64, TransactionID>> prepared_append_ranges_{};

    // Used by dump index / create index
    Map<u64, SharedPtr<TableIndexCache>> index_cache_map_{};
};

} // namespace infinity