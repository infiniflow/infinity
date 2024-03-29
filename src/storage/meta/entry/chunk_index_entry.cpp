// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

#include "type/complex/row_id.h"
#include <cassert>

module chunk_index_entry;

import stl;
import third_party;
import base_entry;
import meta_entry_interface;
import cleanup_scanner;
import segment_index_entry;

namespace infinity {

ChunkIndexEntry::ChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count)
    : BaseEntry(EntryType::kChunkIndex, false), segment_index_entry_(segment_index_entry), base_name_(base_name), base_rowid_(base_rowid),
      row_count_(row_count){};

SharedPtr<ChunkIndexEntry>
ChunkIndexEntry::NewChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count) {
    auto chunk_index_entry = SharedPtr<ChunkIndexEntry>(new ChunkIndexEntry(segment_index_entry, base_name, base_rowid, row_count));
    return chunk_index_entry;
}

nlohmann::json ChunkIndexEntry::Serialize() {
    nlohmann::json index_entry_json;
    index_entry_json["base_name"] = this->base_name_;
    index_entry_json["base_rowid"] = this->base_rowid_.ToUint64();
    index_entry_json["row_count"] = this->row_count_;
    return index_entry_json;
}

UniquePtr<ChunkIndexEntry> ChunkIndexEntry::Deserialize(const nlohmann::json &index_entry_json, SegmentIndexEntry *segment_index_entry) {
    String base_name = index_entry_json["base_name"];
    RowID base_rowid = RowID::FromUint64(index_entry_json["base_rowid"]);
    u32 row_count = index_entry_json["row_count"];
    return MakeUnique<ChunkIndexEntry>(segment_index_entry, base_name, base_rowid, row_count);
}

} // namespace infinity
