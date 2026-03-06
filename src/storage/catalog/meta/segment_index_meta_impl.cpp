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

module infinity_core:segment_index_meta.impl;

import :segment_index_meta;
import :kv_code;
import :kv_store;
import :table_index_meta;
import :table_meta;
import :infinity_context;
import :new_catalog;
import :mem_index;
import :index_base;
import :meta_info;
import :chunk_index_meta;
import :new_txn;
import :txn_state;
import :infinity_exception;
import :utility;
import :memory_indexer;
import :logger;
import :plaid_global_centroids;
import :virtual_store;
import :index_plaid;

import std;
import third_party;

import create_index_info;

namespace infinity {

SegmentIndexMeta::SegmentIndexMeta(SegmentID segment_id, TableIndexMeta &table_index_meta)
    : begin_ts_(table_index_meta.table_meta().begin_ts()), commit_ts_(table_index_meta.table_meta().commit_ts()),
      kv_instance_(table_index_meta.kv_instance()), table_index_meta_(table_index_meta), segment_id_(segment_id) {}

SegmentIndexMeta::~SegmentIndexMeta() = default;

Status SegmentIndexMeta::GetNextChunkID(ChunkID &chunk_id) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!next_chunk_id_) {
        Status status = LoadNextChunkID();
        if (!status.ok()) {
            return status;
        }
    }
    chunk_id = *next_chunk_id_;
    return Status::OK();
}

Status SegmentIndexMeta::SetNextChunkID(ChunkID chunk_id) {
    next_chunk_id_ = chunk_id;
    std::string next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
    std::string next_chunk_id_str = std::to_string(chunk_id);
    Status status = kv_instance_.Put(next_chunk_id_key, next_chunk_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

std::tuple<ChunkID, Status> SegmentIndexMeta::GetAndSetNextChunkID() {
    std::lock_guard<std::mutex> lock(mtx_);
    ChunkID chunk_id = std::numeric_limits<ChunkID>::max();
    if (!next_chunk_id_) {
        Status status = LoadNextChunkID();
        // If next_chunk_id is not found, it means the segment index is empty.
        // We can use chunk id 0 and set the next_chunk_id to 1.
        if (status.code_ == ErrorCode::kNotFound) {
            next_chunk_id_ = 0;
        } else if (!status.ok()) {
            return {chunk_id, status};
        }
    }
    chunk_id = *next_chunk_id_;

    (*next_chunk_id_)++;
    std::string next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
    std::string next_chunk_id_str = std::to_string(*next_chunk_id_);
    Status status = kv_instance_.Put(next_chunk_id_key, next_chunk_id_str);

    return {chunk_id, status};
}

std::tuple<std::vector<ChunkID> *, Status> SegmentIndexMeta::GetChunkIDs1() {
    if (!chunk_ids_) {
        auto status = LoadChunkIDs1();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {&*chunk_ids_, Status::OK()};
}

Status SegmentIndexMeta::RemoveChunkIDs(const std::vector<ChunkID> &chunk_ids) {
    TableMeta &table_meta = table_index_meta_.table_meta();
    for (ChunkID chunk_id : chunk_ids) {
        std::string chunk_id_key =
            KeyEncode::CatalogIdxChunkKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, chunk_id);
        Status status = kv_instance_.Delete(chunk_id_key);
        if (!status.ok()) {
            return status;
        }

        for (auto it = chunk_ids_->begin(); it != chunk_ids_->end(); ++it) {
            if (*it == chunk_id) {
                chunk_ids_->erase(it);
                break;
            }
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::AddChunkIndexID1(ChunkID chunk_id, NewTxn *new_txn) {

    TableMeta &table_meta = table_index_meta_.table_meta();
    std::string chunk_id_key =
        KeyEncode::CatalogIdxChunkKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, chunk_id);
    std::string commit_ts_str;
    switch (new_txn->GetTxnState()) {
        case TxnState::kStarted: {
            commit_ts_str = "-1"; // Wait for commit
            new_txn->AddMetaKeyForCommit(chunk_id_key);
            break;
        }
        case TxnState::kCommitting:
        case TxnState::kCommitted: {
            commit_ts_str = fmt::format("{}", new_txn->CommitTS());
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid transaction state: {}", TxnState2Str(new_txn->GetTxnState())));
        }
    }
    if (!chunk_ids_) {
        chunk_ids_ = std::vector<ChunkID>();
    }
    chunk_ids_->push_back(chunk_id);
    return kv_instance_.Put(chunk_id_key, commit_ts_str);
}

Status SegmentIndexMeta::InitSet1() {
    // next_chunk_id is not set in InitSet1, it will be set when the first chunk is added.
    return Status::OK();
}

Status SegmentIndexMeta::RestoreSet(const ChunkID &next_chunk_id) { return SetNextChunkID(next_chunk_id); }

Status SegmentIndexMeta::LoadSet() {
    {
        auto [index_base, status] = table_index_meta_.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::UninitSet1(UsageFlag usage_flag) {
    {
        // Remove all chunk ids
        TableMeta &table_meta = table_index_meta_.table_meta();
        std::string chunk_id_prefix =
            KeyEncode::CatalogIdxChunkPrefix(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_);
        auto iter = kv_instance_.GetIterator();
        iter->Seek(chunk_id_prefix);
        while (iter->Valid() && iter->Key().starts_with(chunk_id_prefix)) {
            kv_instance_.Delete(iter->Key().ToString());
            iter->Next();
        }
        chunk_ids_.reset();
    }
    {
        // Remove next chunk id
        std::string next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
        Status status = kv_instance_.Delete(next_chunk_id_key);
        if (!status.ok()) {
            return status;
        }
        next_chunk_id_.reset();
    }
    if (usage_flag == UsageFlag::kOther) {
        // Clear mem index
        std::shared_ptr<MemIndex> mem_index = GetMemIndex();
        if (mem_index != nullptr) {
            mem_index->ClearMemIndex();
        }

        // Remove mem index
        std::string mem_index_key = GetSegmentIndexTag("mem_index");
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        Status status = new_catalog->DropMemIndexByMemIndexKey(mem_index_key);
        if (!status.ok()) {
            return status;
        }
    }
    {
        // Remove ft_info tag
        std::string ft_info_key = GetSegmentIndexTag("ft_info");
        Status status = kv_instance_.Delete(ft_info_key);
        if (!status.ok()) {
            return status;
        }
        ft_info_.reset();
    }
    return Status::OK();
}

std::shared_ptr<MemIndex> SegmentIndexMeta::GetMemIndex(bool for_update) {
    std::string mem_index_key = GetSegmentIndexTag("mem_index");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    return new_catalog->GetMemIndex(mem_index_key, for_update);
}

std::shared_ptr<MemIndex> SegmentIndexMeta::PopMemIndex() {
    std::string mem_index_key = GetSegmentIndexTag("mem_index");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    return new_catalog->PopMemIndex(mem_index_key);
}

Status SegmentIndexMeta::LoadChunkIDs1() {
    chunk_ids_ = std::vector<ChunkID>();
    std::vector<ChunkID> &chunk_ids = *chunk_ids_;
    TxnTimeStamp begin_ts = table_index_meta_.table_meta().begin_ts();
    TxnTimeStamp commit_ts = table_index_meta_.table_meta().commit_ts();

    TableMeta &table_meta = table_index_meta_.table_meta();
    std::string chunk_id_prefix =
        KeyEncode::CatalogIdxChunkPrefix(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(chunk_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(chunk_id_prefix)) {
        std::string key = iter->Key().ToString();
        auto [chunk_id, is_chunk_id] = ExtractU64FromStringSuffix(key, chunk_id_prefix.size());
        if (is_chunk_id) {
            TxnTimeStamp chunk_commit_ts = std::stoull(iter->Value().ToString());
            if (chunk_commit_ts > begin_ts && chunk_commit_ts != commit_ts && chunk_commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                iter->Next();
                continue;
            }
            // the key is committed before the txn or the key isn't committed
            chunk_ids.push_back(chunk_id);
        }
        iter->Next();
    }

    std::ranges::sort(chunk_ids);
    return Status::OK();
}

Status SegmentIndexMeta::LoadNextChunkID() {
    std::string next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
    std::string next_chunk_id_str;
    Status status = kv_instance_.Get(next_chunk_id_key, next_chunk_id_str);
    if (!status.ok()) {
        return status;
    }
    next_chunk_id_ = std::stoull(next_chunk_id_str);
    return Status::OK();
}

std::string SegmentIndexMeta::GetSegmentIndexTag(const std::string &tag) {
    const TableMeta &table_meta = table_index_meta_.table_meta();
    return KeyEncode::CatalogIdxSegmentTagKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, tag);
}

std::shared_ptr<std::string> SegmentIndexMeta::GetSegmentIndexDir() const {
    std::shared_ptr<std::string> table_index_dir = table_index_meta_.GetTableIndexDir();
    if (!table_index_dir) {
        return nullptr;
    }
    return std::make_shared<std::string>(fmt::format("{}/seg_{}", *table_index_dir, segment_id_));
}

std::shared_ptr<SegmentIndexInfo> SegmentIndexMeta::GetSegmentIndexInfo() {
    std::shared_ptr<IndexBase> index_def;
    Status status;
    std::tie(index_def, status) = table_index_meta_.GetIndexBase();
    if (!status.ok()) {
        return nullptr;
    }
    if (!chunk_ids_) {
        Status status = LoadChunkIDs1();
        if (!status.ok()) {
            return nullptr;
        }
    }
    std::vector<std::string> segment_index_files;
    for (auto &chunk_id : *chunk_ids_) {
        ChunkIndexMeta chunk_index_meta(chunk_id, *this);
        std::vector<std::string> chunk_index_files;
        status = chunk_index_meta.FilePaths(chunk_index_files);
        if (!status.ok()) {
            return nullptr;
        }
        segment_index_files.insert(segment_index_files.end(), chunk_index_files.begin(), chunk_index_files.end());
    }

    std::shared_ptr<SegmentIndexInfo> segment_index_info = std::make_shared<SegmentIndexInfo>();
    segment_index_info->segment_id_ = segment_id_;
    segment_index_info->index_type_ = index_def->index_type_;
    segment_index_info->index_dir_ = GetSegmentIndexDir();
    segment_index_info->chunk_count_ = chunk_ids_.value().size();
    segment_index_info->files_ = std::move(segment_index_files);
    return segment_index_info;
}

std::tuple<std::shared_ptr<SegmentIndexSnapshotInfo>, Status> SegmentIndexMeta::MapMetaToSnapShotInfo() {
    std::shared_ptr<SegmentIndexSnapshotInfo> segment_index_snapshot_info = std::make_shared<SegmentIndexSnapshotInfo>();
    segment_index_snapshot_info->segment_id_ = segment_id_;
    auto [chunk_ids, status] = GetChunkIDs1();
    if (!status.ok()) {
        return {nullptr, status};
    }
    for (auto &chunk_id : *chunk_ids) {
        ChunkIndexMeta chunk_index_meta(chunk_id, *this);
        auto [chunk_index_snapshot, chunk_index_status] = chunk_index_meta.MapMetaToSnapShotInfo(chunk_id);
        if (!chunk_index_status.ok()) {
            return {nullptr, chunk_index_status};
        }
        segment_index_snapshot_info->chunk_index_snapshots_.push_back(chunk_index_snapshot);
    }
    return {segment_index_snapshot_info, Status::OK()};
}

// PLAID global centroids management
Status SegmentIndexMeta::SetPlaidGlobalCentroids(const std::shared_ptr<PlaidGlobalCentroids> &centroids) {
    std::lock_guard<std::mutex> lock(mtx_);
    plaid_global_centroids_ = centroids;
    return Status::OK();
}

std::shared_ptr<PlaidGlobalCentroids> SegmentIndexMeta::GetPlaidGlobalCentroids() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!plaid_global_centroids_) {
        // Try to load from disk
        Status status = LoadPlaidGlobalCentroids();
        if (!status.ok()) {
            return nullptr;
        }
    }
    return plaid_global_centroids_;
}

Status SegmentIndexMeta::SavePlaidGlobalCentroids() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!plaid_global_centroids_) {
        return Status::OK(); // Nothing to save
    }

    // Save to file in segment index directory
    auto segment_dir = GetSegmentIndexDir();
    if (!segment_dir) {
        return Status::UnexpectedError("Failed to get segment index directory");
    }

    // Ensure directory exists
    if (!VirtualStore::Exists(*segment_dir)) {
        Status mkdir_status = VirtualStore::MakeDirectory(*segment_dir);
        if (!mkdir_status.ok()) {
            return mkdir_status;
        }
    }

    std::string centroids_file = fmt::format("{}/plaid_centroids.bin", *segment_dir);

    // Use VirtualStore to create file handle
    auto [file_handle, status] = VirtualStore::Open(centroids_file, FileAccessMode::kWrite);
    if (!status.ok()) {
        return status;
    }

    plaid_global_centroids_->Save(*file_handle);
    file_handle->Sync();

    LOG_INFO(fmt::format("Saved PLAID global centroids to {}", centroids_file));
    return Status::OK();
}

Status SegmentIndexMeta::LoadPlaidGlobalCentroids() {
    // Note: This method is called with mtx_ already held from GetPlaidGlobalCentroids
    // or called without lock from UninitSet1

    auto segment_dir = GetSegmentIndexDir();
    if (!segment_dir) {
        return Status::UnexpectedError("Failed to get segment index directory");
    }

    std::string centroids_file = fmt::format("{}/plaid_centroids.bin", *segment_dir);

    // Check if file exists
    if (!VirtualStore::Exists(centroids_file)) {
        return Status::NotFound("PLAID global centroids file not found");
    }

    // Get index info for dimension and nbits
    auto [index_base, index_status] = table_index_meta_.GetIndexBase();
    if (!index_status.ok()) {
        return index_status;
    }

    if (index_base->index_type_ != IndexType::kPLAID) {
        return Status::UnexpectedError("Not a PLAID index");
    }

    // Get embedding dimension from column def
    auto [column_def, col_status] = table_index_meta_.GetColumnDef();
    if (!col_status.ok()) {
        return col_status;
    }

    const auto *embedding_info = static_cast<EmbeddingInfo *>(column_def->type()->type_info().get());
    const u32 embedding_dimension = embedding_info->Dimension();

    // Get nbits from index base
    const auto *index_plaid = static_cast<const IndexPLAID *>(index_base.get());
    const u32 nbits = index_plaid->nbits_;

    // Create and load centroids
    plaid_global_centroids_ = std::make_shared<PlaidGlobalCentroids>(embedding_dimension, nbits);

    auto [file_handle, status] = VirtualStore::Open(centroids_file, FileAccessMode::kRead);
    if (!status.ok()) {
        plaid_global_centroids_.reset();
        return status;
    }

    plaid_global_centroids_->Load(*file_handle);

    LOG_INFO(fmt::format("Loaded PLAID global centroids from {} (n_centroids={})", centroids_file, plaid_global_centroids_->n_centroids()));
    return Status::OK();
}

bool SegmentIndexMeta::HasPlaidGlobalCentroids() {
    std::lock_guard<std::mutex> lock(mtx_);
    return plaid_global_centroids_ != nullptr && plaid_global_centroids_->IsTrained();
}

} // namespace infinity
