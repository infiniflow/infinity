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

import third_party;
import index_def;
import status;
import extra_ddl_info;

namespace infinity {

class TxnManager;
class BufferManager;
struct TableEntry;
struct SegmentEntry;

class TableIndexMeta {
    friend struct TableEntry;

public:
    explicit TableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name);

    static UniquePtr<TableIndexMeta> NewTableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name);

public:
    // Getter
    inline TableEntry *GetTableEntry() const { return table_entry_; }

    Tuple<TableIndexEntry *, Status> GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<TableIndexEntry *, Status> GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);

private:
    Tuple<TableIndexEntry *, Status> CreateTableIndexEntry(const SharedPtr<IndexDef> &index_def,
                                                           ConflictType conflict_type,
                                                           TransactionID txn_id,
                                                           TxnTimeStamp begin_ts,
                                                           TxnManager *txn_mgr,
                                                           bool is_replay,
                                                           String replay_table_index_dir);

    Tuple<TableIndexEntry *, Status>
    DropTableIndexEntry(ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<TableIndexMeta> Deserialize(const nlohmann::json &index_def_meta_json, TableEntry *table_entry, BufferManager *buffer_mgr);

    void DeleteNewEntry(TransactionID txn_id, TxnManager *txn_mgr);

    void MergeFrom(TableIndexMeta &other);

    Tuple<TableIndexEntry *, Status> CreateTableIndexEntryInternal(const SharedPtr<IndexDef> &index_def,
                                                                   TransactionID txn_id,
                                                                   TxnTimeStamp begin_ts,
                                                                   TxnManager *txn_mgr,
                                                                   bool is_replay,
                                                                   String replay_table_index_dir);

    Tuple<TableIndexEntry *, Status> DropTableIndexEntryInternal(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

public:
    String index_name() const { return *index_name_; }
    List<SharedPtr<BaseEntry>> &entry_list() { return entry_list_; }

private:
    SharedPtr<String> index_name_{};
    TableEntry *table_entry_{};

    std::shared_mutex rw_locker_{};
    List<SharedPtr<BaseEntry>> entry_list_{};
};
} // namespace infinity