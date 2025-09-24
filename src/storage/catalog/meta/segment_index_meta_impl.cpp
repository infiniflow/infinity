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
import :table_index_meeta;
import :table_meeta;
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

import std;
import third_party;

import create_index_info;

namespace infinity {

SegmentIndexMeta::SegmentIndexMeta(SegmentID segment_id, TableIndexMeeta &table_index_meta)
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
    TableMeeta &table_meta = table_index_meta_.table_meta();
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

    TableMeeta &table_meta = table_index_meta_.table_meta();
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

Status SegmentIndexMeta::SetNoMemIndex() {
    std::string has_mem_index_key = GetSegmentIndexTag("has_mem_index");
    std::string has_mem_index_str = "0";
    // block here when mem index is inserting.
    Status status = kv_instance_.Put(has_mem_index_key, has_mem_index_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status SegmentIndexMeta::InitSet1() {
    // next_chunk_id is not set in InitSet1, it will be set when the first chunk is added.
    return Status::OK();
}

Status SegmentIndexMeta::RestoreSet(const ChunkID &next_chunk_id) { return SetNextChunkID(next_chunk_id); }

Status SegmentIndexMeta::LoadSet() {
    // {
    //     std::string mem_index_key = GetSegmentIndexTag("mem_index");
    //     NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    //     Status status = new_catalog->AddMemIndex(std::move(mem_index_key), std::make_shared<MemIndex>());
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    // {
    //     Status status = SetNoMemIndex();
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    {
        auto [index_base, status] = table_index_meta_.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::UninitSet(UsageFlag usage_flag) {
    {
        std::string next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
        Status status = kv_instance_.Delete(next_chunk_id_key);
        if (!status.ok()) {
            return status;
        }
        next_chunk_id_.reset();
    }
    if (usage_flag == UsageFlag::kOther) {
        {
            std::string mem_index_key = GetSegmentIndexTag("mem_index");
            NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
            Status status = new_catalog->DropMemIndexByMemIndexKey(mem_index_key);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        // std::string has_mem_index_key = GetSegmentIndexTag("has_mem_index");
        // Status status = kv_instance_.Delete(has_mem_index_key);
        // if (!status.ok()) {
        //     return status;
        // }
    }
    {
        std::string ft_info_key = GetSegmentIndexTag("ft_info");
        Status status = kv_instance_.Delete(ft_info_key);
        if (!status.ok()) {
            return status;
        }
        ft_info_.reset();
    }
    return Status::OK();
}

Status SegmentIndexMeta::UninitSet1(UsageFlag usage_flag) {
    {
        // Remove all chunk ids
        TableMeeta &table_meta = table_index_meta_.table_meta();
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
        // Remove mem index indicator
        // std::string has_mem_index_key = GetSegmentIndexTag("has_mem_index");
        // Status status = kv_instance_.Delete(has_mem_index_key);
        // if (!status.ok()) {
        //     return status;
        // }
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

bool SegmentIndexMeta::HasMemIndex() {
    std::string mem_index_key = GetSegmentIndexTag("mem_index");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    return new_catalog->HasMemIndex(mem_index_key);
}

Status SegmentIndexMeta::LoadChunkIDs1() {
    chunk_ids_ = std::vector<ChunkID>();
    std::vector<ChunkID> &chunk_ids = *chunk_ids_;
    TxnTimeStamp begin_ts = table_index_meta_.table_meta().begin_ts();
    TxnTimeStamp commit_ts = table_index_meta_.table_meta().commit_ts();

    TableMeeta &table_meta = table_index_meta_.table_meta();
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

    std::sort(chunk_ids.begin(), chunk_ids.end());
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
    const TableMeeta &table_meta = table_index_meta_.table_meta();
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

} // namespace infinity
