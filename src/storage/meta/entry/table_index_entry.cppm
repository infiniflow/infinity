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

export module catalog:table_index_entry;

import :column_index_entry;
import :segment_column_index_entry;
import :irs_index_entry;
import :base_entry;

import stl;
import index_def;
import third_party;

namespace infinity {

class TableIndexMeta;
class BufferManager;
struct TableEntry;

struct TableIndexEntry : public BaseEntry {

    friend struct TableEntry;

public:
    TableIndexEntry(const SharedPtr<IndexDef> &index_def,
                    TableIndexMeta *table_index_meta,
                    SharedPtr<String> index_dir,
                    u64 txn_id,
                    TxnTimeStamp begin_ts,
                    bool replay = false);

    TableIndexEntry(TableIndexMeta *table_index_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static UniquePtr<TableIndexEntry>
    NewTableIndexEntry(const SharedPtr<IndexDef> &index_def, TableIndexMeta *table_index_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static UniquePtr<TableIndexEntry> NewDropTableIndexEntry(TableIndexMeta *table_index_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static Json Serialize(TableIndexEntry *table_index_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<TableIndexEntry>
    Deserialize(const Json &index_def_entry_json, TableIndexMeta *table_index_meta, BufferManager *buffer_mgr, TableEntry *table_entry);

public:
    // Getter
    inline const TableIndexMeta *table_index_meta() const { return table_index_meta_; }
    inline const IndexDef *index_def() const { return index_def_.get(); }
    const SharedPtr<IrsIndexEntry> &irs_index_entry() const { return irs_index_entry_; }
    HashMap<u64, UniquePtr<ColumnIndexEntry>> &column_index_map() { return column_index_map_; }

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

    void CommitCreateIndex(u64 column_id, u32 segment_id, SharedPtr<SegmentColumnIndexEntry> index_entry);
    void CommitCreateIndex(const SharedPtr<IrsIndexEntry> &irs_index_entry);

private:
    RWMutex rw_locker_{};
    TableIndexMeta *table_index_meta_{};
    const SharedPtr<IndexDef> index_def_{};
    SharedPtr<String> index_dir_{};

    HashMap<u64, UniquePtr<ColumnIndexEntry>> column_index_map_{};

    SharedPtr<IrsIndexEntry> irs_index_entry_{};
};

} // namespace infinity
