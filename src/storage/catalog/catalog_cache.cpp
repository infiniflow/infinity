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
import index_base;

namespace infinity {

TableCache::TableCache(u64 table_id, SegmentID unsealed_segment_id, SegmentOffset unsealed_segment_offset, SegmentID next_segment_id)
    : prepare_segment_id_(unsealed_segment_id), prepare_segment_offset_(unsealed_segment_offset), commit_segment_id_(unsealed_segment_id),
      commit_segment_offset_(unsealed_segment_offset), table_id_(table_id), next_segment_id_(next_segment_id) {}

SharedPtr<AppendPrepareInfo> TableCache::PrepareAppendNolock(SizeT row_count, TransactionID txn_id) {
    if (row_count > MAX_BLOCK_CAPACITY or row_count == 0) {
        UnrecoverableError(fmt::format("Attempt to append row_count: {}", row_count));
    }

    SharedPtr<AppendPrepareInfo> append_info = MakeShared<AppendPrepareInfo>();
    append_info->transaction_id_ = txn_id;
    if (unsealed_segment_cache_ == nullptr) {
        unsealed_segment_cache_ = MakeShared<SegmentCache>(next_segment_id_, row_count);
        unsealed_segment_cache_->sealed_ = false;

        // Update prepare info
        prepare_segment_id_ = next_segment_id_;
        prepare_segment_offset_ = row_count;

        append_info->ranges_.emplace_back(RowID(next_segment_id_, 0), row_count);
        ++next_segment_id_;
    } else {
        if (unsealed_segment_cache_->row_count_ + row_count < DEFAULT_SEGMENT_CAPACITY) {
            // Don't need to add new segment
            append_info->ranges_.emplace_back(RowID(unsealed_segment_cache_->segment_id_, unsealed_segment_cache_->row_count_), row_count);
            unsealed_segment_cache_->row_count_ += row_count;

            prepare_segment_offset_ += row_count;
        } else if (unsealed_segment_cache_->row_count_ + row_count == DEFAULT_SEGMENT_CAPACITY) {
            // Need to add new segment
            append_info->ranges_.emplace_back(RowID(unsealed_segment_cache_->segment_id_, unsealed_segment_cache_->row_count_), row_count);
            unsealed_segment_cache_->row_count_ += row_count;
            unsealed_segment_cache_->sealed_ = true;
            sealed_segment_cache_map_.emplace(unsealed_segment_cache_->segment_id_, unsealed_segment_cache_);

            prepare_segment_offset_ += row_count;
        } else {
            SizeT first_row_count = DEFAULT_SEGMENT_CAPACITY - unsealed_segment_cache_->row_count_;
            append_info->ranges_.emplace_back(RowID(unsealed_segment_cache_->segment_id_, unsealed_segment_cache_->row_count_), first_row_count);
            unsealed_segment_cache_->row_count_ += first_row_count;
            unsealed_segment_cache_->sealed_ = true;
            sealed_segment_cache_map_.emplace(unsealed_segment_cache_->segment_id_, unsealed_segment_cache_);

            // Update prepare info
            SizeT second_row_count = row_count - first_row_count;
            prepare_segment_id_ = next_segment_id_;
            prepare_segment_offset_ = second_row_count;

            // create new segment
            unsealed_segment_cache_ = MakeShared<SegmentCache>(next_segment_id_, second_row_count);
            unsealed_segment_cache_->sealed_ = false;
            append_info->ranges_.emplace_back(RowID(next_segment_id_, 0), second_row_count);
            ++next_segment_id_;
        }
    }

    uncommitted_append_infos_.emplace_back(append_info);
    return append_info;
}

void TableCache::CommitAppendNolock(const SharedPtr<AppendPrepareInfo> &append_info, TransactionID txn_id) {
    SharedPtr<AppendPrepareInfo> saved_append_info = uncommitted_append_infos_.front();
    if (saved_append_info->transaction_id_ != txn_id) {
        UnrecoverableError(fmt::format("Attempt to commit append prepare transaction id: {} != {}", saved_append_info->transaction_id_, txn_id));
    }
    uncommitted_append_infos_.pop_front();
    const Vector<Pair<RowID, u64>> &ranges = append_info->ranges_;
    for (const auto &range : ranges) {
        commit_segment_id_ = range.first.segment_id_;
        commit_segment_offset_ += range.second;
        if (commit_segment_offset_ > DEFAULT_SEGMENT_CAPACITY) {
            commit_segment_offset_ -= DEFAULT_SEGMENT_CAPACITY;
        }
    }
    return;
}

bool TableCache::AllPrepareAreCommittedNolock() const {
    return prepare_segment_id_ == commit_segment_id_ && prepare_segment_offset_ == commit_segment_offset_;
}

RowID TableCache::GetCommitPosition() const { return RowID(commit_segment_id_, commit_segment_offset_); }

// Import segments
SharedPtr<ImportPrepareInfo> TableCache::PrepareImportSegmentsNolock(u64 segment_count, TransactionID txn_id) {
    SharedPtr<ImportPrepareInfo> import_prepare_info = MakeShared<ImportPrepareInfo>();
    // Get the prepared segment id and insert into the import prepare info
    for (SizeT i = 0; i < segment_count; ++i) {
        SegmentID segment_id = next_segment_id_;
        import_prepare_info->segment_ids_.emplace_back(segment_id);
        ++next_segment_id_;
    }

    Vector<SegmentID> &segments = import_prepare_info->segment_ids_;

    // Create index prepare info
    for (const auto &index_pair : index_cache_map_) {
        auto index_cache = index_pair.second;
        CreateIndexPrepareInfo index_prepare_info;
        index_prepare_info.index_id_ = index_cache->index_id_;
        index_prepare_info.index_name_ = index_cache->index_name_;
        index_prepare_info.segment_index_prepare_infos_.reserve(segment_count);
        for (const SegmentID segment_id : segments) {
            SegmentIndexPrepareInfo segment_index_prepare_info;
            segment_index_prepare_info.segment_id_ = segment_id;
            segment_index_prepare_info.chunk_id_ = 0; // chunk id should be zero
            segment_index_prepare_info.row_count_ = segment_cache_map_[segment_id]->row_count_;
            index_prepare_info.segment_index_prepare_infos_.emplace_back(segment_index_prepare_info);
        }
        import_prepare_info->indexes_.emplace_back(index_prepare_info);
    }

    import_prepare_info_map_.emplace(txn_id, import_prepare_info);
    return import_prepare_info;
}

void TableCache::CommitImportSegmentsNolock(const SharedPtr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id) {
    auto iter = import_prepare_info_map_.find(txn_id);
    if (iter == import_prepare_info_map_.end()) {
        UnrecoverableError(fmt::format("Transaction id: {} not found in import prepare info map", txn_id));
    }
    import_prepare_info_map_.erase(iter);

    // Commit the segments
    SizeT segment_count = import_prepare_info->segment_ids_.size();
    for (SizeT idx = 0; idx < segment_count; ++idx) {
        SegmentID segment_id = import_prepare_info->segment_ids_[idx];
        u64 row_count = import_prepare_info->row_counts_[idx];

        auto iter = segment_cache_map_.find(segment_id);
        if (iter != segment_cache_map_.end()) {
            UnrecoverableError(fmt::format("Segment id: {} already exists in segment cache map", segment_id));
        }
        SegmentCache *segment_cache = iter->second.get();
        segment_cache->segment_id_ = segment_id;
        segment_cache->sealed_ = true;
        segment_cache->row_count_ = row_count;
    }

    // Commit segment index
    for (const auto &index_prepare_info : import_prepare_info->indexes_) {
        auto index_iter = index_cache_map_.find(index_prepare_info.index_id_);
        if (index_iter == index_cache_map_.end()) {
            UnrecoverableError(fmt::format("Index id: {} not found in index cache map", index_prepare_info.index_id_));
        }
        TableIndexCache *index_cache = index_iter->second.get();
        for (const auto &segment_index_prepare_info : index_prepare_info.segment_index_prepare_infos_) {
            SegmentID segment_id = segment_index_prepare_info.segment_id_;
            auto segment_iter = index_cache->segment_index_cache_map_.find(segment_id);
            if (segment_iter != index_cache->segment_index_cache_map_.end()) {
                UnrecoverableError(fmt::format("Segment id: {} already exists in segment cache map", segment_id));
            }
            auto segment_index_cache = MakeShared<SegmentIndexCache>(segment_id);
            segment_index_cache->next_chunk_id_ = segment_index_prepare_info.chunk_id_ + 1;
            Pair<RowID, u64> range = {RowID(segment_id, 0), segment_index_prepare_info.row_count_};
            segment_index_cache->chunk_row_ranges_.emplace(segment_index_prepare_info.chunk_id_, range);
        }
    }
    return;
}

// Compact segments
Tuple<SharedPtr<CompactPrepareInfo>, Status> TableCache::PrepareCompactSegmentsNolock(const Vector<SegmentID> &segment_ids) {
    return {nullptr, Status::OK()};
}
void TableCache::CommitCompactSegmentsNolock(const SharedPtr<CompactPrepareInfo> &import_prepare_info) { return; }

// Optimize segments
Tuple<SharedPtr<OptimizePrepareInfo>, Status> TableCache::PrepareOptimizeSegmentsNolock(const Vector<SegmentID> &segment_ids) {
    return {nullptr, Status::OK()};
}
Tuple<SharedPtr<OptimizePrepareInfo>, Status> TableCache::PrepareOptimizeSegmentsNolock() { return {nullptr, Status::OK()}; }
void TableCache::CommitOptimizeSegmentsNolock(const SharedPtr<OptimizePrepareInfo> &import_prepare_info) { return; }

Tuple<SharedPtr<ImportPrepareInfo>, Status> TableCache::GetNewSegmentIDsNolock(SizeT segment_count) {
    //    Vector<SegmentID> segment_ids;
    //    segment_ids.reserve(segment_count);
    //    for (SizeT i = 0; i < segment_count; ++i) {
    //        segment_ids.emplace_back(next_segment_id_);
    //        ++next_segment_id_;
    //    }
    //    return segment_ids;
    return {nullptr, Status::OK()};
}

Pair<RowID, u64> TableCache::PrepareDumpIndexRangeNolock(u64 index_id) { return {RowID(), 0}; }

void TableCache::AddTableIndexCacheNolock(const SharedPtr<TableIndexCache> &table_index_cache) {
    auto [iter, insert_success] = index_cache_map_.emplace(table_index_cache->index_id_, table_index_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Table index cache with id: {} already exists", table_index_cache->index_id_));
    }
}

void TableCache::DropTableIndexCacheNolock(u64 index_id) {
    auto iter = index_cache_map_.find(index_id);
    if (iter == index_cache_map_.end()) {
        LOG_ERROR(fmt::format("Table index cache with id: {} not found", index_id));
    }
    index_cache_map_.erase(iter);
}

void DbCache::AddNewTableCacheNolock(u64 table_id, const String &table_name) {
    SharedPtr<TableCache> table_cache = MakeShared<TableCache>(table_id, table_name);
    auto [iter, insert_success] = table_cache_map_.emplace(table_cache->table_id(), table_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Table cache with id: {} already exists", table_cache->table_id()));
    }
}

void DbCache::AddTableCacheNolock(const SharedPtr<TableCache> &table_cache) {
    auto [iter, insert_success] = table_cache_map_.emplace(table_cache->table_id(), table_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Table cache with id: {} already exists", table_cache->table_id()));
    }
}

void DbCache::DropTableCacheNolock(u64 table_id) {
    auto iter = table_cache_map_.find(table_id);
    if (iter == table_cache_map_.end()) {
        LOG_ERROR(fmt::format("Table cache with id: {} not found", table_id));
    }
    table_cache_map_.erase(iter);
}

void SystemCache::AddNewDbCache(const String &db_name, u64 db_id) {
    std::unique_lock lock(cache_mtx_);
    auto db_cache = MakeShared<DbCache>(db_id, db_name, 0);
    Status status = this->AddDbCacheNolock(db_cache);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

void SystemCache::DropDbCache(u64 db_id) {
    std::unique_lock lock(cache_mtx_);
    auto cache_iter = db_cache_map_.find(db_id);
    if (cache_iter == db_cache_map_.end()) {
        LOG_ERROR(fmt::format("Db cache with id: {} not found", db_id));
    }
    String db_name = cache_iter->second->db_name();
    db_cache_map_.erase(cache_iter);
    auto name_iter = db_name_map_.find(db_name);
    if (name_iter == db_name_map_.end()) {
        LOG_ERROR(fmt::format("Db name cache with name: {} not found", cache_iter->second->db_name()));
    }
    db_name_map_.erase(name_iter);
}

void SystemCache::AddNewTableCache(u64 db_id, u64 table_id, const String &table_name) {
    std::unique_lock lock(cache_mtx_);
    DbCache *db_cache = this->GetDbCacheNolock(db_id);
    db_cache->AddNewTableCacheNolock(table_id, table_name);
}

void SystemCache::DropTableCache(u64 db_id, u64 table_id) {
    std::unique_lock lock(cache_mtx_);
    DbCache *db_cache = this->GetDbCacheNolock(db_id);
    db_cache->DropTableCacheNolock(table_id);
}

SharedPtr<ImportPrepareInfo> SystemCache::PrepareImportSegments(u64 db_id, u64 table_id, u64 segment_count, TransactionID txn_id) {
    std::unique_lock lock(cache_mtx_);
    TableCache *table_cache = this->GetTableCacheNolock(db_id, table_id);
    return table_cache->PrepareImportSegmentsNolock(segment_count, txn_id);
}

void SystemCache::CommitImportSegments(u64 db_id, u64 table_id, const SharedPtr<ImportPrepareInfo> &import_prepare_info, TransactionID txn_id) {
    std::unique_lock lock(cache_mtx_);
    TableCache *table_cache = this->GetTableCacheNolock(db_id, table_id);
    table_cache->CommitImportSegmentsNolock(import_prepare_info, txn_id);
}

Tuple<u64, Status> SystemCache::AddNewIndexCache(u64 db_id, u64 table_id, const String &index_name) {
    std::unique_lock lock(cache_mtx_);
    TableCache *table_cache = this->GetTableCacheNolock(db_id, table_id);
    u64 index_id = table_cache->next_index_id_;
    SharedPtr<TableIndexCache> table_index_cache = MakeShared<TableIndexCache>(db_id, table_id, index_id, index_name);
    table_cache->AddTableIndexCacheNolock(table_index_cache);
    ++table_cache->next_index_id_;
    return {0, Status::OK()};
}

void SystemCache::DropIndexCache(u64 db_id, u64 table_id, u64 index_id) {
    std::unique_lock lock(cache_mtx_);
    TableCache *table_cache = this->GetTableCacheNolock(db_id, table_id);
    auto index_iter = table_cache->index_cache_map_.find(index_id);
    if (index_iter == table_cache->index_cache_map_.end()) {
        UnrecoverableError(fmt::format("Table index cache with id: {} not found", index_id));
    }
    table_cache->DropTableIndexCacheNolock(index_id);
}

SharedPtr<AppendPrepareInfo> SystemCache::PrepareAppend(u64 db_id, u64 table_id, SizeT row_count, TransactionID txn_id) {
    std::unique_lock lock(cache_mtx_);
    TableCache *table_cache = this->GetTableCacheNolock(db_id, table_id);
    return table_cache->PrepareAppendNolock(row_count, txn_id);
}
void SystemCache::CommitAppend(u64 db_id, u64 table_id, const SharedPtr<AppendPrepareInfo> &append_info, TransactionID txn_id) {
    std::unique_lock lock(cache_mtx_);
    TableCache *table_cache = this->GetTableCacheNolock(db_id, table_id);
    table_cache->CommitAppendNolock(append_info, txn_id);
}

Status SystemCache::AddDbCacheNolock(const SharedPtr<DbCache> &db_cache) {
    auto [iter2, insert_success2] = db_name_map_.emplace(db_cache->db_name(), db_cache->db_id());
    if (!insert_success2) {
        return Status::DuplicateDatabase(db_cache->db_name());
    }
    auto [iter, insert_success] = db_cache_map_.emplace(db_cache->db_id(), db_cache);
    if (!insert_success) {
        UnrecoverableError(fmt::format("Db cache with id: {} already exists", db_cache->db_id()));
    }
    return Status::OK();
}

SharedPtr<DbCache> SystemCache::GetDbCache(u64 db_id) const {
    std::unique_lock lock(cache_mtx_);
    auto iter = db_cache_map_.find(db_id);
    if (iter == db_cache_map_.end()) {
        return nullptr;
    }
    return iter->second;
}

DbCache *SystemCache::GetDbCacheNolock(u64 db_id) {
    auto db_iter = db_cache_map_.find(db_id);
    if (db_iter == db_cache_map_.end()) {
        UnrecoverableError(fmt::format("Db cache with id: {} not found", db_id));
    }
    DbCache *db_cache = db_iter->second.get();
    return db_cache;
}

TableCache *SystemCache::GetTableCacheNolock(u64 db_id, u64 table_id) {
    DbCache *db_cache = this->GetDbCacheNolock(db_id);

    auto table_iter = db_cache->table_cache_map_.find(table_id);
    if (table_iter == db_cache->table_cache_map_.end()) {
        UnrecoverableError(fmt::format("Table cache with id: {} not found", table_id));
    }
    TableCache *table_cache = table_iter->second.get();
    return table_cache;
}

nlohmann::json SystemCache::ToJson() const {
    nlohmann::json result;
    std::unique_lock lock(cache_mtx_);
    return result;
}

} // namespace infinity