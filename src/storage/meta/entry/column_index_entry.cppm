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

import stl;
import parser;
import base_index;
import base_entry;
import third_party;
import segment_column_index_entry;
import base_index;

export module column_index_entry;

namespace infinity {

class BufferManager;
class TableCollectionEntry;
class TableIndexEntry;

export struct ColumnIndexEntry : public BaseEntry {
public:
    explicit ColumnIndexEntry(SharedPtr<BaseIndex> base_index,
                              TableIndexEntry *table_index_entry,
                              SharedPtr<String> index_dir,
                              u64 txn_id,
                              TxnTimeStamp begin_ts);

    static SharedPtr<ColumnIndexEntry> NewColumnIndexEntry(SharedPtr<BaseIndex> base_index,
                                                           u64 column_id,
                                                           TableIndexEntry *table_index_entry,
                                                           u64 txn_id,
                                                           SharedPtr<String> index_dir,
                                                           TxnTimeStamp begin_ts);

    static void CommitCreatedIndex(ColumnIndexEntry *column_index_entry, u32 segment_id, UniquePtr<SegmentColumnIndexEntry> index_entry);

    static Json Serialize(const ColumnIndexEntry *column_index_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<ColumnIndexEntry> Deserialize(const Json &column_index_entry_json,
                                                   TableIndexEntry *table_index_entry,
                                                   BufferManager *buffer_mgr,
                                                   TableCollectionEntry *table_entry);

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

public:
    RWMutex rw_locker_{};

    TableIndexEntry *table_index_entry_{};
    SharedPtr<String> index_dir_{};
    const SharedPtr<BaseIndex> base_index_{};
    HashMap<u32, SharedPtr<SegmentColumnIndexEntry>> index_by_segment{};
};
} // namespace infinity
