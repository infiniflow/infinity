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

#include <string>

module segment_index_meta;

import kv_code;
import kv_store;
import table_index_meeta;
import table_meeta;
import third_party;
import infinity_context;
import new_catalog;
import mem_index;
import index_base;
import create_index_info;
import meta_info;
import chunk_index_meta;
import new_txn;
import txn_state;
import infinity_exception;
import utility;
import memory_indexer;

namespace infinity {

SegmentIndexMeta::SegmentIndexMeta(SegmentID segment_id, TableIndexMeeta &table_index_meta)
    : kv_instance_(table_index_meta.kv_instance()), table_index_meta_(table_index_meta), segment_id_(segment_id) {
    begin_ts_ = table_index_meta_.table_meta().begin_ts();
}

SegmentIndexMeta::~SegmentIndexMeta() = default;

Tuple<Vector<ChunkID> *, Status> SegmentIndexMeta::GetChunkIDs() {
    if (!chunk_ids_) {
        Status status = LoadChunkIDs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {&*chunk_ids_, Status::OK()};
}

Status SegmentIndexMeta::GetNextChunkID(ChunkID &chunk_id) {
    if (!next_chunk_id_) {
        Status status = LoadNextChunkID();
        if (!status.ok()) {
            return status;
        }
    }
    chunk_id = *next_chunk_id_;
    return Status::OK();
}

Tuple<ChunkID, Status> SegmentIndexMeta::GetNextChunkID1() {
    if (!next_chunk_id_) {
        Status status = LoadNextChunkID();
        if (!status.ok()) {
            return {std::numeric_limits<ChunkID>::max(), status};
        }
    }
    return {*next_chunk_id_, Status::OK()};
}

Tuple<Vector<ChunkID> *, Status> SegmentIndexMeta::GetChunkIDs1() {
    if (!chunk_ids_) {
        auto status = LoadChunkIDs1();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {&*chunk_ids_, Status::OK()};
}

Status SegmentIndexMeta::GetFtInfo(SharedPtr<SegmentIndexFtInfo> &ft_info) {
    if (!ft_info_) {
        Status status = LoadFtInfo();
        if (!status.ok()) {
            return status;
        }
    }
    ft_info = ft_info_;
    return Status::OK();
}

Status SegmentIndexMeta::SetChunkIDs(const Vector<ChunkID> &chunk_ids) {
    chunk_ids_ = chunk_ids;
    String chunk_ids_key = GetSegmentIndexTag("chunk_ids");
    String chunk_ids_str = nlohmann::json(chunk_ids).dump();
    Status status = kv_instance_.Put(chunk_ids_key, chunk_ids_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status SegmentIndexMeta::RemoveChunkIDs(const Vector<ChunkID> &chunk_ids) {
    TableMeeta &table_meta = table_index_meta_.table_meta();
    for (ChunkID chunk_id : chunk_ids) {
        String chunk_id_key =
            KeyEncode::CatalogIdxChunkKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, chunk_id);
        Status status = kv_instance_.Delete(chunk_id_key);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::AddChunkID(ChunkID chunk_id) {
    if (!chunk_ids_) {
        Status status = LoadChunkIDs();
        if (!status.ok()) {
            return status;
        }
    }
    chunk_ids_->push_back(chunk_id);
    Status status = SetChunkIDs(*chunk_ids_);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status SegmentIndexMeta::AddChunkIndexID1(ChunkID chunk_id, NewTxn *new_txn) {

    TableMeeta &table_meta = table_index_meta_.table_meta();
    String chunk_id_key =
        KeyEncode::CatalogIdxChunkKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, chunk_id);
    String commit_ts_str;
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
        chunk_ids_ = Vector<ChunkID>();
    }
    chunk_ids_->push_back(chunk_id);
    return kv_instance_.Put(chunk_id_key, commit_ts_str);
}

Status SegmentIndexMeta::SetNextChunkID(ChunkID chunk_id) {
    next_chunk_id_ = chunk_id;
    String next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
    String next_chunk_id_str = std::to_string(chunk_id);
    Status status = kv_instance_.Put(next_chunk_id_key, next_chunk_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status SegmentIndexMeta::SetFtInfo(const SharedPtr<SegmentIndexFtInfo> &ft_info) {
    ft_info_ = ft_info;
    String ft_info_key = GetSegmentIndexTag("ft_info");

    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->AddSegmentIndexFtInfo(ft_info_key, ft_info);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status SegmentIndexMeta::UpdateFtInfo(u64 column_len_sum, u32 column_len_cnt) {
    if (!ft_info_) {
        Status status = LoadFtInfo();
        if (!status.ok()) {
            return status;
        }
    }
    ft_info_->ft_column_len_sum_ += column_len_sum;
    ft_info_->ft_column_len_cnt_ += column_len_cnt;
    return Status::OK();
}

Status SegmentIndexMeta::SetNoMemIndex() {
    String has_mem_index_key = GetSegmentIndexTag("has_mem_index");
    String has_mem_index_str = "0";
    // block here when mem index is inserting.
    Status status = kv_instance_.Put(has_mem_index_key, has_mem_index_str);
    if (!status.ok()) {
        return status;
    }
    has_mem_index_ = false;
    return Status::OK();
}

Status SegmentIndexMeta::InitSet() {
    {
        Status status = SetChunkIDs(Vector<ChunkID>());
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetNextChunkID(0);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String mem_index_key = GetSegmentIndexTag("mem_index");
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        Status status = new_catalog->AddMemIndex(std::move(mem_index_key), MakeShared<MemIndex>());
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetNoMemIndex();
        if (!status.ok()) {
            return status;
        }
    }
    {
        auto [index_base, status] = table_index_meta_.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
        if (index_base->index_type_ == IndexType::kFullText) {
            auto ft_info = MakeShared<SegmentIndexFtInfo>();
            Status status = this->SetFtInfo(ft_info);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::InitSet1() {
    {
        Status status = SetNextChunkID(0);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String mem_index_key = GetSegmentIndexTag("mem_index");
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        Status status = new_catalog->AddMemIndex(std::move(mem_index_key), MakeShared<MemIndex>());
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetNoMemIndex();
        if (!status.ok()) {
            return status;
        }
    }
    {
        auto [index_base, status] = table_index_meta_.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
        if (index_base->index_type_ == IndexType::kFullText) {
            auto ft_info = MakeShared<SegmentIndexFtInfo>();
            Status status = this->SetFtInfo(ft_info);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::LoadSet() {
    {
        String mem_index_key = GetSegmentIndexTag("mem_index");
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        Status status = new_catalog->AddMemIndex(std::move(mem_index_key), MakeShared<MemIndex>());
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetNoMemIndex();
        if (!status.ok()) {
            return status;
        }
    }
    {
        auto [index_base, status] = table_index_meta_.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
        if (index_base->index_type_ == IndexType::kFullText) {
            auto ft_info = MakeShared<SegmentIndexFtInfo>();
            Status status = this->SetFtInfo(ft_info);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status SegmentIndexMeta::UninitSet(UsageFlag usage_flag) {
    {
        String chunk_ids_key = GetSegmentIndexTag("chunk_ids");
        Status status = kv_instance_.Delete(chunk_ids_key);
        if (!status.ok()) {
            return status;
        }
        chunk_ids_.reset();
    }
    {
        String next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
        Status status = kv_instance_.Delete(next_chunk_id_key);
        if (!status.ok()) {
            return status;
        }
        next_chunk_id_.reset();
    }
    if (usage_flag == UsageFlag::kOther) {
        {
            String mem_index_key = GetSegmentIndexTag("mem_index");
            NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
            Status status = new_catalog->DropMemIndexByMemIndexKey(mem_index_key);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        String has_mem_index_key = GetSegmentIndexTag("has_mem_index");
        Status status = kv_instance_.Delete(has_mem_index_key);
        if (!status.ok()) {
            return status;
        }
        has_mem_index_.reset();
    }
    {
        String ft_info_key = GetSegmentIndexTag("ft_info");
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
        String chunk_id_prefix =
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
        String next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
        Status status = kv_instance_.Delete(next_chunk_id_key);
        if (!status.ok()) {
            return status;
        }
        next_chunk_id_.reset();
    }
    if (usage_flag == UsageFlag::kOther) {
        {
            // Remove mem index
            String mem_index_key = GetSegmentIndexTag("mem_index");
            NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
            Status status = new_catalog->DropMemIndexByMemIndexKey(mem_index_key);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        // Remove mem index indicator
        String has_mem_index_key = GetSegmentIndexTag("has_mem_index");
        Status status = kv_instance_.Delete(has_mem_index_key);
        if (!status.ok()) {
            return status;
        }
        has_mem_index_.reset();
    }
    {
        // Remove ft_info tag
        String ft_info_key = GetSegmentIndexTag("ft_info");
        Status status = kv_instance_.Delete(ft_info_key);
        if (!status.ok()) {
            return status;
        }
        ft_info_.reset();
    }
    return Status::OK();
}

Status SegmentIndexMeta::GetMemIndex(SharedPtr<MemIndex> &mem_index) {
    mem_index.reset();
    bool has_mem_index = false;
    {
        Status status = GetHasMemIndex(has_mem_index);
        if (!status.ok()) {
            return status;
        }
    }

    // append -> dump -> clear memindex -> new txn { append -> mem index(visible) -> put rocksdb has_mem_index before commit rocksdb.
    // At same time, another full text commit thread will commit mem index, since rocksdb isn't commit has_mem_index flag, the memindex will be
    // removed below following code.
    if (!has_mem_index) {
        mem_index = MakeShared<MemIndex>();
        return Status::OK();
    }

    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    String mem_index_key = GetSegmentIndexTag("mem_index");
    Status status = new_catalog->GetMemIndex(mem_index_key, mem_index);
    if (!status.ok()) {
        return status;
    }
    if (!has_mem_index) {
        mem_index->ClearMemIndex();
    }

    return Status::OK();
}

Status SegmentIndexMeta::GetMemIndexRaw(SharedPtr<MemIndex> &mem_index) {
    mem_index.reset();

    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    String mem_index_key = GetSegmentIndexTag("mem_index");
    Status status = new_catalog->GetMemIndex(mem_index_key, mem_index);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status SegmentIndexMeta::GetAndWriteMemIndex(SharedPtr<MemIndex> &mem_index) {
    // Check has mem index key, if no, set the key.
    // Clear mem index if no such key
    // This function is used by populate index and append index (used by append or recover mem index)
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();

    String has_mem_index_key = GetSegmentIndexTag("has_mem_index");
    bool has_mem_index = false;
    {
        String has_mem_index_str;
        // block here when mem index is dumpping.
        // Use GetForUpdate here. because if we use Get, when `has_mem_index_key` is true, we skip mem_index->Clear() here.
        // when we dump mem index simultaneously, we do append and dump on the same mem index.
        Status status = kv_instance_.Get(has_mem_index_key, has_mem_index_str);
        if (!status.ok()) {
            return status;
        }
        has_mem_index = std::stoi(has_mem_index_str);
    }

    if (!has_mem_index) {
        String has_mem_index_str = "1";
        {
            Status status = kv_instance_.Put(has_mem_index_key, has_mem_index_str);
            if (!status.ok()) {
                return status;
            }
        }
        has_mem_index_ = true;
    }
    {
        String mem_index_key = GetSegmentIndexTag("mem_index");
        Status status = new_catalog->GetMemIndex(mem_index_key, mem_index);
        if (!status.ok()) {
            return status;
        }
    }
    if (!has_mem_index) {
        // Clear here because dump mem index may not clear after commit. Dump index might be conflict with AppendIndex, this is to clear the mem index
        // before new append data is construct to new index
        mem_index->ClearMemIndex();
    }
    return Status::OK();
}

Status SegmentIndexMeta::GetHasMemIndex(bool &has_mem_index) {
    if (!has_mem_index_) {
        String has_mem_index_key = GetSegmentIndexTag("has_mem_index");
        String has_mem_index_str;
        Status status = kv_instance_.Get(has_mem_index_key, has_mem_index_str);
        if (!status.ok()) {
            return status;
        }
        has_mem_index_ = std::stoi(has_mem_index_str);
    }
    has_mem_index = *has_mem_index_;
    return Status::OK();
}

Status SegmentIndexMeta::LoadChunkIDs() {
    String chunk_ids_key = GetSegmentIndexTag("chunk_ids");
    String chunk_ids_str;
    Status status = kv_instance_.Get(chunk_ids_key, chunk_ids_str);
    if (!status.ok()) {
        return status;
    }
    chunk_ids_ = nlohmann::json::parse(chunk_ids_str).get<Vector<ChunkID>>();
    return Status::OK();
}

Status SegmentIndexMeta::LoadChunkIDs1() {
    chunk_ids_ = Vector<ChunkID>();
    Vector<ChunkID> &chunk_ids = *chunk_ids_;
    TxnTimeStamp begin_ts = table_index_meta_.table_meta().begin_ts();

    TableMeeta &table_meta = table_index_meta_.table_meta();
    String chunk_id_prefix =
        KeyEncode::CatalogIdxChunkPrefix(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(chunk_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(chunk_id_prefix)) {
        String key = iter->Key().ToString();
        auto [chunk_id, is_chunk_id] = ExtractU64FromStringSuffix(key, chunk_id_prefix.size());
        if (is_chunk_id) {
            TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
            if (commit_ts > begin_ts and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
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
    String next_chunk_id_key = GetSegmentIndexTag("next_chunk_id");
    String next_chunk_id_str;
    Status status = kv_instance_.Get(next_chunk_id_key, next_chunk_id_str);
    if (!status.ok()) {
        return status;
    }
    next_chunk_id_ = std::stoull(next_chunk_id_str);
    return Status::OK();
}

Status SegmentIndexMeta::LoadFtInfo() {
    String ft_info_key = GetSegmentIndexTag("ft_info");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->GetSegmentIndexFtInfo(ft_info_key, ft_info_);

    if (ft_info_->ft_column_len_sum_ == 0) {
        SharedPtr<MemIndex> mem_index;
        Status status = GetMemIndex(mem_index);
        if (status.ok() && mem_index && mem_index->memory_indexer_) {
            ft_info_->ft_column_len_sum_ = mem_index->memory_indexer_->GetColumnLengthSum();
            ft_info_->ft_column_len_cnt_ = mem_index->memory_indexer_->GetDocCount();
        }
    }

    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

String SegmentIndexMeta::GetSegmentIndexTag(const String &tag) {
    const TableMeeta &table_meta = table_index_meta_.table_meta();
    return KeyEncode::CatalogIdxSegmentTagKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, tag);
}

SharedPtr<String> SegmentIndexMeta::GetSegmentIndexDir() const {
    SharedPtr<String> table_index_dir = table_index_meta_.GetTableIndexDir();
    if (!table_index_dir) {
        return nullptr;
    }
    return MakeShared<String>(fmt::format("{}/seg_{}", *table_index_dir, segment_id_));
}

SharedPtr<SegmentIndexInfo> SegmentIndexMeta::GetSegmentIndexInfo() {
    SharedPtr<IndexBase> index_def;
    Status status;
    std::tie(index_def, status) = table_index_meta_.GetIndexBase();
    if (!status.ok()) {
        return nullptr;
    }
    if (!chunk_ids_) {
        Status status = LoadChunkIDs();
        if (!status.ok()) {
            return nullptr;
        }
    }
    Vector<String> segment_index_files;
    for (auto &chunk_id : *chunk_ids_) {
        ChunkIndexMeta chunk_index_meta(chunk_id, *this);
        Vector<String> chunk_index_files;
        status = chunk_index_meta.FilePaths(chunk_index_files);
        if (!status.ok()) {
            return nullptr;
        }
        segment_index_files.insert(segment_index_files.end(), chunk_index_files.begin(), chunk_index_files.end());
    }

    SharedPtr<SegmentIndexInfo> segment_index_info = MakeShared<SegmentIndexInfo>();
    segment_index_info->segment_id_ = segment_id_;
    segment_index_info->index_type_ = index_def->index_type_;
    segment_index_info->index_dir_ = GetSegmentIndexDir();
    segment_index_info->chunk_count_ = chunk_ids_.value().size();
    segment_index_info->files_ = std::move(segment_index_files);
    return segment_index_info;
}

} // namespace infinity
