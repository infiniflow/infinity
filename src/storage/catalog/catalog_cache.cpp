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

#include <vector>

module catalog_cache;

import stl;
import internal_types;
import default_values;
import infinity_exception;
import third_party;
import logger;

namespace infinity {

TableCache::TableCache(u64 db_id, u64 table_id, SegmentID unsealed_segment_id, SegmentOffset unsealed_segment_offset, SegmentID next_segment_id)
    : db_id_(db_id), table_id_(table_id), prepare_unsealed_segment_id_(unsealed_segment_id),
      prepare_unsealed_segment_offset_(unsealed_segment_offset), commit_unsealed_segment_id_(unsealed_segment_id),
      commit_unsealed_segment_offset_(unsealed_segment_offset), next_segment_id_(next_segment_id) {
    if (unsealed_segment_offset == DEFAULT_SEGMENT_CAPACITY) {
        all_segments_sealed_prepare_ = true;
        all_segments_sealed_commit_ = true;
    } else {
        all_segments_sealed_prepare_ = false;
        all_segments_sealed_commit_ = false;
    }
}

Vector<Pair<RowID, u64>> TableCache::PrepareAppendRanges(SizeT row_count, TransactionID transaction_id) {
    if (row_count > MAX_BLOCK_CAPACITY) {
        UnrecoverableError(fmt::format("Attempt to prepare row_count: {} > MAX_BLOCK_CAPACITY: {}", row_count, MAX_BLOCK_CAPACITY));
    }

    Vector<Pair<RowID, u64>> ranges;
    if (all_segments_sealed_prepare_) {
        prepare_unsealed_segment_id_ = next_segment_id_;
        prepare_unsealed_segment_offset_ = row_count;
        ++next_segment_id_;
        ranges.emplace_back(RowID(prepare_unsealed_segment_id_, 0), row_count);
        all_segments_sealed_prepare_ = false;
    } else {
        if (prepare_unsealed_segment_offset_ + row_count < DEFAULT_SEGMENT_CAPACITY) {
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, prepare_unsealed_segment_offset_), row_count);
            prepare_unsealed_segment_offset_ += row_count;
            all_segments_sealed_prepare_ = false;
        } else if (prepare_unsealed_segment_offset_ + row_count == DEFAULT_SEGMENT_CAPACITY) {
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, prepare_unsealed_segment_offset_), row_count);
            prepare_unsealed_segment_offset_ += row_count;
            all_segments_sealed_prepare_ = true;
        } else {
            SizeT remaining = DEFAULT_SEGMENT_CAPACITY - prepare_unsealed_segment_offset_;
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, prepare_unsealed_segment_offset_), remaining);
            prepare_unsealed_segment_id_ = next_segment_id_;
            ++next_segment_id_;
            prepare_unsealed_segment_offset_ = row_count - remaining; // remaining rows must not exceed DEFAULT_SEGMENT_CAPACITY
            ranges.emplace_back(RowID(prepare_unsealed_segment_id_, 0), prepare_unsealed_segment_offset_);
            all_segments_sealed_prepare_ = false;
        }
    }

    // append all ranges item to prepared_append_ranges_
    for (const auto &range : ranges) {
        prepared_append_ranges_.emplace_back(range.first, range.second, transaction_id);
    }
    return ranges;
}

void TableCache::CommitAppendRanges(const Vector<Pair<RowID, u64>> &ranges, TransactionID txn_id) {
    for (const auto &range : ranges) {
        auto [row_offset, row_count, transaction] = prepared_append_ranges_.front();
        if (range.first != row_offset or range.second != row_count or transaction != txn_id) {
            UnrecoverableError(fmt::format("Attempt to commit row range: {}: {}, {} != {}: {}, {}",
                                           txn_id,
                                           range.first.ToString(),
                                           range.second,
                                           transaction,
                                           row_offset.ToString(),
                                           row_count));
        }
        prepared_append_ranges_.pop_front();
        commit_unsealed_segment_id_ = range.first.segment_id_;
        commit_unsealed_segment_offset_ += range.second;
        if (commit_unsealed_segment_offset_ == DEFAULT_SEGMENT_CAPACITY) {
            all_segments_sealed_commit_ = true;
        } else if (commit_unsealed_segment_offset_ > DEFAULT_SEGMENT_CAPACITY) {
            commit_unsealed_segment_offset_ -= DEFAULT_SEGMENT_CAPACITY;
            all_segments_sealed_commit_ = false;
        } else {
            all_segments_sealed_commit_ = false;
        }
    }
}

RowID TableCache::GetCommitUnsealedPosition() {
    if (all_segments_sealed_commit_) {
        UnrecoverableError("No unsealed segment");
    }
    return RowID(commit_unsealed_segment_id_, commit_unsealed_segment_offset_);
}

Vector<SegmentID> TableCache::GetImportSegments(SizeT segment_count) {
    Vector<SegmentID> segment_ids;
    segment_ids.reserve(segment_count);
    for (SizeT i = 0; i < segment_count; ++i) {
        segment_ids.emplace_back(next_segment_id_);
        ++next_segment_id_;
    }
    return segment_ids;
}

SegmentID TableCache::GetCompactSegment() {
    // For debug, not to use next_segment_id_++
    SegmentID segment_id = next_segment_id_;
    next_segment_id_ += 1;
    return segment_id;
}

Pair<RowID, u64> TableCache::PrepareDumpIndexRange(u64 index_id) { return {RowID(), 0}; }

void TableCache::AddTableIndexCache(const SharedPtr<TableIndexCache> &table_index_cache) {
    auto [iter, insert_success] = index_cache_map_.emplace(table_index_cache->index_id(), table_index_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Table index cache with id: {} already exists", table_index_cache->index_id()));
    }
}

void TableCache::DropTableIndexCache(u64 index_id) {
    auto iter = index_cache_map_.find(index_id);
    if (iter == index_cache_map_.end()) {
        LOG_ERROR(fmt::format("Table index cache with id: {} not found", index_id));
    }
    index_cache_map_.erase(iter);
}

u64 DbCache::RequestNextTableID() {
    u64 table_id = next_table_id_;
    ++next_table_id_;
    return table_id;
}

void DbCache::AddTableCache(const SharedPtr<TableCache> &table_cache) {
    auto [iter, insert_success] = table_cache_map_.emplace(table_cache->table_id(), table_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Table cache with id: {} already exists", table_cache->table_id()));
    }
}
void DbCache::DropTableCache(u64 table_id) {
    auto iter = table_cache_map_.find(table_id);
    if (iter == table_cache_map_.end()) {
        LOG_ERROR(fmt::format("Table cache with id: {} not found", table_id));
    }
    table_cache_map_.erase(iter);
}

u64 SystemCache::RequestNextDbID() {
    u64 db_id = next_db_id_;
    ++next_db_id_;
    return db_id;
}

void SystemCache::AddDbCache(const SharedPtr<DbCache> &db_cache) {
    auto [iter, insert_success] = db_cache_map_.emplace(db_cache->db_id(), db_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Db cache with id: {} already exists", db_cache->db_id()));
    }
}

SharedPtr<DbCache> SystemCache::GetDbCache(u64 db_id) const {
    auto iter = db_cache_map_.find(db_id);
    if (iter == db_cache_map_.end()) {
        return nullptr;
    }
    return iter->second;
}

void SystemCache::DropDbCache(u64 db_id) {
    auto iter = db_cache_map_.find(db_id);
    if (iter == db_cache_map_.end()) {
        LOG_ERROR(fmt::format("Db cache with id: {} not found", db_id));
    }
    db_cache_map_.erase(iter);
}

} // namespace infinity