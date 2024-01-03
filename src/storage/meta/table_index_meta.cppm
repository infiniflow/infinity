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

export module catalog:table_index_meta;

import :table_index_entry;
import :base_entry;

import stl;
import parser;
import third_party;
import index_def;
import status;

namespace infinity {

class TxnManager;
class BufferManager;
struct TableEntry;
struct SegmentEntry;

class TableIndexMeta {
    friend struct TableEntry;

public:
    explicit TableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name);

public:
    // Getter
    inline TableEntry *GetTableEntry() const { return table_entry_; }

    Tuple<TableIndexEntry *, Status> GetEntry(u64 txn_id, TxnTimeStamp begin_ts);

private:
    Tuple<TableIndexEntry *, Status>
    CreateTableIndexEntry(const SharedPtr<IndexDef> &index_def, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableIndexEntry *, Status> DropTableIndexEntry(ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static SharedPtr<String> ToString(TableIndexMeta *table_index_meta);

    static Json Serialize(TableIndexMeta *table_index_meta, TxnTimeStamp max_commit_ts);

    static UniquePtr<TableIndexMeta> Deserialize(const Json &index_def_meta_json, TableEntry *table_entry, BufferManager *buffer_mgr);

    void DeleteNewEntry(u64 txn_id, TxnManager *txn_mgr);

    void MergeFrom(TableIndexMeta &other);

    Tuple<TableIndexEntry *, Status>
    CreateTableIndexEntryInternal(const SharedPtr<IndexDef> &index_def, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableIndexEntry *, Status> DropTableIndexEntryInternal(u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

private:
    //    RWMutex rw_locker_{};
    SharedPtr<String> index_name_{};
    TableEntry *table_entry_{};

    RWMutex rw_locker_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};
} // namespace infinity