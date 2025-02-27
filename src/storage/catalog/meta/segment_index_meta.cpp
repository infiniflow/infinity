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

namespace infinity {

void SegmentIndexFtInfo::ToJson(nlohmann::json &json) const {
    json["ft_column_len_sum"] = ft_column_len_sum_;
    json["ft_column_len_cnt"] = ft_column_len_cnt_;
}

void SegmentIndexFtInfo::FromJson(const nlohmann::json &json) {
    ft_column_len_sum_ = json["ft_column_len_sum"].get<u64>();
    ft_column_len_cnt_ = json["ft_column_len_cnt"].get<u32>();
}

SegmentIndexMeta::SegmentIndexMeta(SegmentID segment_id, TableIndexMeeta &table_index_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), table_index_meta_(table_index_meta), segment_id_(segment_id) {}

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

Status SegmentIndexMeta::SetFtInfo(const SegmentIndexFtInfo &ft_info) {
    ft_info_ = ft_info;
    String ft_info_key = GetSegmentIndexTag("ft_info");
    nlohmann::json ft_info_json;
    ft_info.ToJson(ft_info_json);
    String ft_info_str = ft_info_json.dump();
    Status status = kv_instance_.Put(ft_info_key, ft_info_str);
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
    Status status = SetFtInfo(*ft_info_);
    if (!status.ok()) {
        return status;
    }
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
    {
        SharedPtr<MemIndex> mem_index;
        Status status = GetMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
        if (mem_index) {
            mem_index->SetToClear();
        }
    }
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
        Status status = SetNoMemIndex ();
        if (!status.ok()) {
            return status;
        }
    }
    {
        SharedPtr<IndexBase> index_def;
        Status status = table_index_meta_.GetIndexDef(index_def);
        if (!status.ok()) {
            return status;
        }
        if (index_def->index_type_ == IndexType::kFullText) {
            SegmentIndexFtInfo ft_info{};
            Status status = this->SetFtInfo(ft_info);
            if (!status.ok()) {
                return status;
            }
        }
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

    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    String mem_index_key = GetSegmentIndexTag("mem_index");
    Status status = new_catalog->GetMemIndex(mem_index_key, mem_index);
    if (!status.ok()) {
        return status;
    }
    if (!has_mem_index) {
        mem_index->Clear();
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
        mem_index->Clear();
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
    String ft_info_str;
    Status status = kv_instance_.Get(ft_info_key, ft_info_str);
    if (!status.ok()) {
        return status;
    }
    nlohmann::json ft_info_json = nlohmann::json::parse(ft_info_str);
    ft_info_ = SegmentIndexFtInfo();
    ft_info_->FromJson(ft_info_json);
    return Status::OK();
}

String SegmentIndexMeta::GetSegmentIndexTag(const String &tag) {
    const TableMeeta &table_meta = table_index_meta_.table_meta();
    return KeyEncode::CatalogIdxSegmentTagKey(table_meta.db_id_str(), table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, tag);
}

} // namespace infinity
