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
import index_file_worker;
import index_base;
import buffer_handle;

namespace infinity {

class Txn;
class SegmentIndexEntry;
struct BlockEntry;
class BufferManager;
class BufferObj;
struct SegmentEntry;

// ChunkIndexEntry is an immutable chunk of SegmentIndexEntry. MemIndexer(for fulltext) is the mutable chunk of SegmentIndexEntry.
export class ChunkIndexEntry : public BaseEntry, public EntryInterface {
private:
    ChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count);

    static String IndexFileName(SegmentID segment_id, const RowID &base_rowid);

    static UniquePtr<IndexFileWorker> CreateFileWorker(const IndexBase *index_base,
                                                       const SharedPtr<String> &index_dir,
                                                       CreateIndexParam *param,
                                                       SegmentID segment_id,
                                                       RowID base_rowid);

public:
    static SharedPtr<ChunkIndexEntry>
    NewChunkIndexEntry(SegmentIndexEntry *segment_index_entry, CreateIndexParam *param, RowID base_rowid, BufferManager *buffer_mgr);

    static SharedPtr<ChunkIndexEntry>
    NewFtChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count);

    static SharedPtr<ChunkIndexEntry> NewReplayChunkIndexEntry(SegmentIndexEntry *segment_index_entry,
                                                               CreateIndexParam *param,
                                                               const String &base_name,
                                                               RowID base_rowid,
                                                               u32 row_count,
                                                               BufferManager *buffer_mgr);

    void SetRowCount(u32 row_count) { row_count_ = row_count; }

    BufferHandle GetIndex();

    nlohmann::json Serialize();

    static UniquePtr<ChunkIndexEntry> Deserialize(const nlohmann::json &index_entry_json, SegmentIndexEntry *segment_index_entry);

    virtual void Cleanup() override;

    virtual void PickCleanup(CleanupScanner *scanner) override {}

    void SaveIndexFile();

public:
    SegmentIndexEntry *segment_index_entry_;
    String base_name_;
    RowID base_rowid_;
    u32 row_count_;

private:
    BufferObj *buffer_obj_{};
};

} // namespace infinity
