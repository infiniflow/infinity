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

module column_meta;

import kv_store;
import kv_code;
import block_meta;
import segment_meta;
import table_meeta;
import third_party;

namespace infinity {

ColumnMeta::ColumnMeta(SizeT column_idx, BlockMeta &block_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), block_meta_(block_meta), column_idx_(column_idx) {}

Status ColumnMeta::SetChunkOffset(SizeT chunk_offset) {
    chunk_offset_ = chunk_offset;
    String chunk_offset_key = GetColumnTag("last_chunk_offset");
    Status status = kv_instance_.Put(chunk_offset_key, fmt::format("{}", *chunk_offset_));
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status ColumnMeta::InitSet() {
    Status status = SetChunkOffset(0);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status ColumnMeta::UninitSet() {
    String chunk_offset_key = GetColumnTag("last_chunk_offset");
    Status status = kv_instance_.Delete(chunk_offset_key);
    if (!status.ok()) {
        return status;
    }
    chunk_offset_.reset();
    return Status::OK();
}

Status ColumnMeta::LoadChunkOffset() {
    String chunk_offset_key = GetColumnTag("last_chunk_offset");
    String chunk_offset_str;
    Status status = kv_instance_.Get(chunk_offset_key, chunk_offset_str);
    if (!status.ok()) {
        return status;
    }
    chunk_offset_ = std::stoull(chunk_offset_str);
    return Status::OK();
}

String ColumnMeta::GetColumnTag(const String &tag) const {
    SegmentMeta &segment_meta = block_meta_.segment_meta();
    TableMeeta &table_meta = segment_meta.table_meta();
    return KeyEncode::CatalogTableSegmentBlockColumnTagKey(table_meta.db_id_str(),
                                                           table_meta.table_id_str(),
                                                           segment_meta.segment_id(),
                                                           block_meta_.block_id(),
                                                           column_idx_,
                                                           tag);
}

} // namespace infinity
