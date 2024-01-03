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

export module catalog:segment_column_index_entry;

import :base_entry;

import stl;
import buffer_handle;
import third_party;
import buffer_obj;
import parser;
import index_file_worker;

namespace infinity {

export struct ColumnIndexEntry;

struct TableEntry;
class FaissIndexPtr;
class BufferManager;
class IndexDef;
struct SegmentEntry;

export class SegmentColumnIndexEntry : public BaseEntry {
    friend ColumnIndexEntry;

public:
    static SharedPtr<SegmentColumnIndexEntry> NewIndexEntry(ColumnIndexEntry *column_index_entry,
                                                            u32 segment_id,
                                                            TxnTimeStamp create_ts,
                                                            BufferManager *buffer_manager,
                                                            CreateIndexParam *create_index_param);

    [[nodiscard]] BufferHandle GetIndex();

    static Json Serialize(SegmentColumnIndexEntry *segment_column_index_entry);

    static UniquePtr<SegmentColumnIndexEntry>
    Deserialize(const Json &index_entry_json, ColumnIndexEntry *column_index_entry, BufferManager *buffer_mgr, TableEntry *table_entry);

    void MergeFrom(BaseEntry &other);

public:
    // Getter
    inline u32 segment_id() const { return segment_id_; }
    inline const ColumnIndexEntry *column_index_entry() const { return column_index_entry_; }

private:
    explicit SegmentColumnIndexEntry(ColumnIndexEntry *column_index_entry, u32 segment_id, BufferObj *buffer);

    void UpdateIndex(TxnTimeStamp commit_ts, FaissIndexPtr *index, BufferManager *buffer_mgr);

    bool Flush(TxnTimeStamp checkpoint_ts);

    // Load from disk. Is called by SegmentColumnIndexEntry::Deserialize.
    static UniquePtr<SegmentColumnIndexEntry>
    LoadIndexEntry(ColumnIndexEntry *column_index_entry, u32 segment_id, BufferManager *buffer_manager, CreateIndexParam *create_index_param);

private:
    const ColumnIndexEntry *column_index_entry_{};
    u32 segment_id_{};

    BufferObj *const buffer_{};

    RWMutex rw_locker_{};

    TxnTimeStamp min_ts_{0}; // Indicate the commit_ts which create this SegmentColumnIndexEntry
    TxnTimeStamp max_ts_{0}; // Indicate the max commit_ts which update data inside this SegmentColumnIndexEntry
    TxnTimeStamp checkpoint_ts_{0};
};

} // namespace infinity
