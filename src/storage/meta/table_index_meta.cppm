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

export module table_index_meta;

import table_index_entry;
import base_entry;
import stl;
import third_party;
import index_base;
import status;
import extra_ddl_info;
import entry_list;
import meta_entry_interface;
import cleanup_scanner;

namespace infinity {

class TxnManager;
class BufferManager;
struct TableEntry;
struct SegmentEntry;

export class TableIndexMeta : public MetaInterface {
    friend struct TableEntry;

public:
    using EntryT = TableIndexEntry;

    explicit TableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name);

    static UniquePtr<TableIndexMeta> NewTableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name);

public:
    // Getter
    inline TableEntry *GetTableEntry() const { return table_entry_; }

    Tuple<TableIndexEntry *, Status> GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<TableIndexEntry *, Status> GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);

private:
    Tuple<TableIndexEntry *, Status> CreateTableIndexEntry(const SharedPtr<IndexBase> &index_base,
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

    Tuple<TableIndexEntry *, Status> CreateTableIndexEntryInternal(const SharedPtr<IndexBase> &index_base,
                                                                   TransactionID txn_id,
                                                                   TxnTimeStamp begin_ts,
                                                                   TxnManager *txn_mgr,
                                                                   bool is_replay,
                                                                   String replay_table_index_dir);

    Tuple<TableIndexEntry *, Status> DropTableIndexEntryInternal(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

public:
    String index_name() const { return *index_name_; }

private:
    SharedPtr<String> index_name_{};
    TableEntry *table_entry_{};

    EntryList<TableIndexEntry> index_entry_list_{};

private:
    // TODO: remove it
    std::shared_mutex &rw_locker() { return index_entry_list_.rw_locker_; }

public:
    // TODO: remove it
    List<SharedPtr<TableIndexEntry>> &index_entry_list() { return index_entry_list_.entry_list_; }

public:
    void Cleanup() && override;

    bool PickCleanup(CleanupScanner *scanner) override ;
};
} // namespace infinity
