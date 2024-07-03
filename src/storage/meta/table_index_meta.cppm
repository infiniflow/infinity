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
import meta_info;

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

    Tuple<TableIndexEntry *, Status> CreateTableIndexEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                           const SharedPtr<IndexBase> &index_base,
                                                           const SharedPtr<String> &table_entry_dir,
                                                           ConflictType conflict_type,
                                                           TransactionID txn_id,
                                                           TxnTimeStamp begin_ts,
                                                           TxnManager *txn_mgr);

    Tuple<SharedPtr<TableIndexEntry>, Status> DropTableIndexEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                                  ConflictType conflict_type,
                                                                  SharedPtr<String> index_name,
                                                                  TransactionID txn_id,
                                                                  TxnTimeStamp begin_ts,
                                                                  TxnManager *txn_mgr);

    Tuple<SharedPtr<TableIndexInfo>, Status>
    GetTableIndexInfo(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<TableIndexEntry *, Status> GetEntry(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
        return index_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    }

    Tuple<TableIndexEntry *, Status> GetEntryNolock(TransactionID txn_id, TxnTimeStamp begin_ts) {
        return index_entry_list_.GetEntryNolock(txn_id, begin_ts);
    }

    void DeleteEntry(TransactionID txn_id);

    // replay
    TableIndexEntry *CreateEntryReplay(std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                                       TransactionID txn_id,
                                       TxnTimeStamp begin_ts);

    void UpdateEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts);

    void DropEntryReplay(std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                         TransactionID txn_id,
                         TxnTimeStamp begin_ts);

    TableIndexEntry *GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);

private:
    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<TableIndexMeta> Deserialize(const nlohmann::json &index_def_meta_json, TableEntry *table_entry, BufferManager *buffer_mgr);

public:
    const SharedPtr<String> &index_name() const { return index_name_; }

    TableEntry *table_entry() const { return table_entry_; }

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
    void Cleanup() override;

    bool PickCleanup(CleanupScanner *scanner) override;

    void PickCleanupBySegments(const Vector<SegmentID> &segment_ids, CleanupScanner *scanner);

    bool Empty() override { return index_entry_list_.Empty(); }
};
} // namespace infinity
