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
// import parser;
import index_def;
import base_entry;
import third_party;
import column_index_entry;
import segment_column_index_entry;
import irs_index_entry;

export module table_index_entry;

namespace infinity {

class TableIndexMeta;
class BufferManager;
// class TxnTableStore;
// class CreateIndexParam;
class TableCollectionEntry;

export class TableIndexEntry : public BaseEntry {

public:
    explicit TableIndexEntry(SharedPtr<IndexDef> index_def,
                             TableIndexMeta *table_index_meta,
                             SharedPtr<String> index_dir,
                             u64 txn_id,
                             TxnTimeStamp begin_ts);

    static UniquePtr<TableIndexEntry>
    NewTableIndexEntry(SharedPtr<IndexDef> index_def, TableIndexMeta *table_index_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static void CommitCreateIndex(TableIndexEntry *table_index_entry, u64 column_id, u32 segment_id, SharedPtr<SegmentColumnIndexEntry> index_entry);
    static void CommitCreateIndex(TableIndexEntry *table_index_entry, SharedPtr<IrsIndexEntry> irs_index_entry);

    static Json Serialize(const TableIndexEntry *table_index_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<TableIndexEntry>
    Deserialize(const Json &index_def_entry_json, TableIndexMeta *table_index_meta, BufferManager *buffer_mgr, TableCollectionEntry *table_entry);

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

public:
    RWMutex rw_locker_{};
    TableIndexMeta *table_index_meta_{};
    const SharedPtr<IndexDef> index_def_{};
    SharedPtr<String> index_dir_{};

    HashMap<u64, SharedPtr<ColumnIndexEntry>> column_index_map_{};

    SharedPtr<IrsIndexEntry> irs_index_entry_{};
};

} // namespace infinity
