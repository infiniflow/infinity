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

module chunk_index_meta;

import kv_code;
import kv_store;
import table_meeta;
import table_index_meeta;
import segment_index_meta;

namespace infinity {

void ChunkIndexMetaInfo::ToJson(nlohmann::json &json) const {
    json["base_name"] = base_name_;
    json["base_row_id"] = base_row_id_.ToUint64();
    json["row_count"] = row_cnt_;
}

void ChunkIndexMetaInfo::FromJson(const nlohmann::json &json) {
    base_name_ = json["base_name"].get<String>();
    base_row_id_ = RowID::FromUint64(json["base_row_id"].get<u64>());
    row_cnt_ = json["row_count"].get<u64>();
}

ChunkIndexMeta::ChunkIndexMeta(ChunkID chunk_id, SegmentIndexMeta &segment_index_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), segment_index_meta_(segment_index_meta), chunk_id_(chunk_id) {}

Status ChunkIndexMeta::InitSet(const ChunkIndexMetaInfo &chunk_info) {
    chunk_info_ = chunk_info;
    {
        String chunk_info_key = GetChunkIndexTag("chunk_info");
        nlohmann::json chunk_info_json;
        chunk_info_->ToJson(chunk_info_json);
        Status status = kv_instance_.Put(chunk_info_key, chunk_info_json.dump());
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status ChunkIndexMeta::LoadChunkInfo() {
    String chunk_info_key = GetChunkIndexTag("chunk_info");
    String chunk_info_str;
    Status s = kv_instance_.Get(chunk_info_key, chunk_info_str);
    if (!s.ok()) {
        return s;
    }
    chunk_info_ = ChunkIndexMetaInfo();
    chunk_info_->FromJson(nlohmann::json::parse(chunk_info_str));
    return Status::OK();
}

String ChunkIndexMeta::GetChunkIndexTag(const String &tag) const {
    const TableIndexMeeta &table_index_meta = segment_index_meta_.table_index_meta();
    const TableMeeta &table_meta = table_index_meta.table_meta();
    return KeyEncode::CatalogIdxChunkTagKey(table_meta.db_id_str_,
                                            table_meta.table_id_str(),
                                            table_index_meta.index_id_str(),
                                            segment_index_meta_.segment_id(),
                                            chunk_id_,
                                            tag);
}

} // namespace infinity
