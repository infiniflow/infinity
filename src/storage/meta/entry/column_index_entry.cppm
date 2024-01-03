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

export module catalog:column_index_entry;

import :base_entry;
import :segment_column_index_entry;

import stl;
import parser;
import index_base;
import third_party;
import index_base;
import index_file_worker;

namespace infinity {

export struct TableIndexEntry;

class BufferManager;
struct TableEntry;

struct ColumnIndexEntry : public BaseEntry {
    friend struct TableEntry;
    friend struct TableIndexEntry;

public:
    ColumnIndexEntry(SharedPtr<IndexBase> index_base,
                     TableIndexEntry *table_index_entry,
                     u64 column_id,
                     SharedPtr<String> index_dir,
                     u64 txn_id,
                     TxnTimeStamp begin_ts);

    static UniquePtr<ColumnIndexEntry> NewColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                                           u64 column_id,
                                                           TableIndexEntry *table_index_entry,
                                                           u64 txn_id,
                                                           SharedPtr<String> index_dir,
                                                           TxnTimeStamp begin_ts);

    static Json Serialize(ColumnIndexEntry *column_index_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<ColumnIndexEntry>
    Deserialize(const Json &column_index_entry_json, TableIndexEntry *table_index_entry, BufferManager *buffer_mgr, TableEntry *table_entry);

public:
    // Getter
    const SharedPtr<String> &index_dir() const { return index_dir_; }
    u64 column_id() const { return column_id_; }
    const IndexBase *index_base_ptr() const { return index_base_.get(); }
    TableIndexEntry *table_index_entry() const { return table_index_entry_; }
    const HashMap<u32, SharedPtr<SegmentColumnIndexEntry>>& index_by_segment() const { return index_by_segment_; }

    // Used in segment column index entry
    UniquePtr<IndexFileWorker> CreateFileWorker(CreateIndexParam *param, u32 segment_id);

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);
    void CommitCreatedIndex(u32 segment_id, UniquePtr<SegmentColumnIndexEntry> index_entry);
    static String IndexFileName(const String &index_name, u32 segment_id);

private:
    RWMutex rw_locker_{};

    TableIndexEntry *table_index_entry_{};
    u64 column_id_{};
    SharedPtr<String> index_dir_{};
    const SharedPtr<IndexBase> index_base_{};
    HashMap<u32, SharedPtr<SegmentColumnIndexEntry>> index_by_segment_{};
};
} // namespace infinity
