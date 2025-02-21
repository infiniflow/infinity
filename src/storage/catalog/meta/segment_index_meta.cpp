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

namespace infinity {

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

String SegmentIndexMeta::GetSegmentIndexTag(const String &tag) {
    const TableMeeta &table_meta = table_index_meta_.table_meta();
    return KeyEncode::CatalogIdxSegmentTagKey(table_meta.db_id_str_, table_meta.table_id_str(), table_index_meta_.index_id_str(), segment_id_, tag);
}

} // namespace infinity
