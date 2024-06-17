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

#include <cassert>

export module chunk_index_entry;

import stl;
import third_party;
import internal_types;
import base_entry;
import meta_entry_interface;
import cleanup_scanner;
import index_file_worker;
import index_base;
import buffer_handle;
import default_values;

namespace infinity {

class Txn;
class SegmentIndexEntry;
struct BlockEntry;
class BufferManager;
class BufferObj;
struct SegmentEntry;

// ChunkIndexEntry is an immutable chunk of SegmentIndexEntry. MemIndexer(for fulltext) is the mutable chunk of SegmentIndexEntry.
export class ChunkIndexEntry : public BaseEntry, public EntryInterface {
public:
    static Vector<std::string_view> DecodeIndex(std::string_view encode);

    static String EncodeIndex(const ChunkID chunk_id, const SegmentIndexEntry *segment_index_entry);

    ChunkIndexEntry(ChunkID chunk_id, SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count);

    static UniquePtr<IndexFileWorker> CreateFileWorker(const IndexBase *index_base,
                                                       const SharedPtr<String> &index_dir,
                                                       CreateIndexParam *param,
                                                       SegmentID segment_id,
                                                       ChunkID chunk_id);

public:
    static String IndexFileName(SegmentID segment_id, ChunkID chunk_id);

    static SharedPtr<ChunkIndexEntry> NewChunkIndexEntry(ChunkID chunk_id,
                                                         SegmentIndexEntry *segment_index_entry,
                                                         CreateIndexParam *param,
                                                         RowID base_rowid,
                                                         BufferManager *buffer_mgr);

    static SharedPtr<ChunkIndexEntry>
    NewFtChunkIndexEntry(SegmentIndexEntry *segment_index_entry, const String &base_name, RowID base_rowid, u32 row_count, BufferManager *buffer_mgr);

    static SharedPtr<ChunkIndexEntry> NewSecondaryIndexChunkIndexEntry(ChunkID chunk_id,
                                                                       SegmentIndexEntry *segment_index_entry,
                                                                       const String &base_name,
                                                                       RowID base_rowid,
                                                                       u32 row_count,
                                                                       BufferManager *buffer_mgr);

    static SharedPtr<ChunkIndexEntry> NewEMVBIndexChunkIndexEntry(ChunkID chunk_id,
                                                                  SegmentIndexEntry *segment_index_entry,
                                                                  const String &base_name,
                                                                  RowID base_rowid,
                                                                  u32 row_count,
                                                                  BufferManager *buffer_mgr);

    static SharedPtr<ChunkIndexEntry> NewReplayChunkIndexEntry(ChunkID chunk_id,
                                                               SegmentIndexEntry *segment_index_entry,
                                                               CreateIndexParam *param,
                                                               const String &base_name,
                                                               RowID base_rowid,
                                                               u32 row_count,
                                                               TxnTimeStamp commit_ts,
                                                               TxnTimeStamp deprecate_ts,
                                                               BufferManager *buffer_mgr);

    void SetRowCount(u32 row_count) { row_count_ = row_count; }

    u32 GetRowCount() const { return row_count_; }

    // Only for fulltext
    u64 GetColumnLengthSum() const;

    inline u32 GetPartNum() const { return (row_count_ + 8191) / 8192; }

    inline u32 GetPartRowCount(const u32 part_id) const { return std::min<u32>(8192, row_count_ - part_id * 8192); }

    BufferHandle GetIndex();

    BufferHandle GetIndexPartAt(u32 i);

    nlohmann::json Serialize();

    static SharedPtr<ChunkIndexEntry>
    Deserialize(const nlohmann::json &index_entry_json, SegmentIndexEntry *segment_index_entry, CreateIndexParam *param, BufferManager *buffer_mgr);

    virtual void Cleanup() override;

    virtual void PickCleanup(CleanupScanner *scanner) override {}

    void SaveIndexFile();

    void LoadPartsReader(BufferManager *buffer_mgr);

    BufferObj *GetBufferObj() { return buffer_obj_; }

    void DeprecateChunk(TxnTimeStamp commit_ts) {
        assert(commit_ts_.load() < commit_ts);
        deprecate_ts_.store(commit_ts);
    }

    bool CheckVisibleByTS(TxnTimeStamp ts) { // FIXME: should overload BaseEntry::CheckVisible
        TxnTimeStamp deprecate_ts = deprecate_ts_.load();
        TxnTimeStamp commit_ts = commit_ts_.load();
        assert(commit_ts == UNCOMMIT_TS || commit_ts < deprecate_ts);
        return ts >= commit_ts && ts <= deprecate_ts;
    }

    bool CheckDeprecate(TxnTimeStamp ts) {
        TxnTimeStamp deprecate_ts = deprecate_ts_.load();
        return ts >= deprecate_ts;
    }

public:
    ChunkID chunk_id_;

    SegmentIndexEntry *segment_index_entry_;
    String base_name_;
    RowID base_rowid_;
    u32 row_count_;
    Atomic<TxnTimeStamp> deprecate_ts_{UNCOMMIT_TS};

private:
    BufferObj *buffer_obj_{};
    Vector<BufferObj *> part_buffer_objs_;
};

} // namespace infinity
