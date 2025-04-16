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

namespace infinity {

SegmentIndexMeta::SegmentIndexMeta(SegmentID segment_id, TableIndexMeeta &table_index_meta)
    : kv_instance_(table_index_meta.kv_instance()), table_index_meta_(table_index_meta), segment_id_(segment_id) {
    begin_ts_ = table_index_meta_.table_meta().begin_ts();
}

SegmentIndexMeta::~SegmentIndexMeta() = default;

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

Tuple<ChunkID, Status> SegmentIndexMeta::AddChunkID1(TxnTimeStamp commit_ts) {
    ChunkID chunk_id = 0;

    auto [chunk_ids_ptr, status] = GetChunkIDs1();
    if (!status.ok()) {
        return {0, status};
    }
    chunk_id = chunk_ids_ptr->empty() ? 0 : chunk_ids_ptr->back() + 1;
    chunk_ids_->push_back(chunk_id);

    TableMeeta &table_meta = table_index_meta_.table_meta();

    String segment_id_key =
        KeyEncode::CatalogIdxChunkKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, chunk_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance_.Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {chunk_id, Status::OK()};
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
    return Status::OK();
}

Status SegmentIndexMeta::UninitSet() {
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
    {
        String mem_index_key = GetSegmentIndexTag("mem_index");
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        Status status = new_catalog->DropMemIndexByMemIndexKey(mem_index_key);
        if (!status.ok()) {
            return status;
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

Status SegmentIndexMeta::GetAndWriteMemIndex(SharedPtr<MemIndex> &mem_index) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();

    String has_mem_index_key = GetSegmentIndexTag("has_mem_index");
    bool has_mem_index = false;
    {
        String has_mem_index_str;
        // block here when mem index is dumpping.
        // Use GetForUpdate here. because if we use Get, when `has_mem_index_key` is true, we skip mem_index->Clear() here.
        // when we dump mem index simultaneously, we do append and dump on the same mem index.
        Status status = kv_instance_.GetForUpdate(has_mem_index_key, has_mem_index_str);
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
        // Clear here because dump mem index may not clear after commit.
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
    Vector<SegmentID> &chunk_ids = *chunk_ids_;

    TableMeeta &table_meta = table_index_meta_.table_meta();
    //    CatalogIdxChunkPrefix(const String &db_id, const String &table_id, const String &index_id, SegmentID segment_id);
    String chunk_id_prefix =
        KeyEncode::CatalogIdxChunkPrefix(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(chunk_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(chunk_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts > begin_ts_) {
            iter->Next();
            continue;
        }
        ChunkID chunk_id = std::stoull(iter->Key().ToString().substr(chunk_id_prefix.size()));
        chunk_ids.push_back(chunk_id);
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
