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

export module chunk_index_entry;

import stl;
import third_party;
import base_entry;
import meta_entry_interface;
import cleanup_scanner;

namespace infinity {

class Txn;
class SegmentIndexEntry;
struct BlockEntry;
struct TableEntry;
class BufferManager;
struct SegmentEntry;

// ChunkIndexEntry is an immutable chunk of SegmentIndexEntry. MemIndexer(for fulltext) is the mutable chunk of SegmentIndexEntry.
export class ChunkIndexEntry : public BaseEntry, public EntryInterface {

public:
    ChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count);

    static SharedPtr<ChunkIndexEntry>
    NewChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count);

    static SharedPtr<ChunkIndexEntry>
    NewReplayChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count);

    nlohmann::json Serialize();

    static UniquePtr<ChunkIndexEntry> Deserialize(const nlohmann::json &index_entry_json, SegmentIndexEntry *segment_index_entry);

    virtual void Cleanup() override {}
    virtual void PickCleanup(CleanupScanner *scanner) override {}

public:
    SegmentIndexEntry *segment_index_entry_;
    String base_name_;
    RowID base_rowid_;
    u32 row_count_;
};

} // namespace infinity
