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

export module table_meta;

import stl;

import third_party;
import table_entry_type;
import buffer_manager;
import status;
import extra_ddl_info;
import column_def;
import base_entry;

import table_entry;
import entry_list;
import meta_info;
import meta_entry_interface;
import cleanup_scanner;

namespace infinity {

class DBEntry;
class TxnManager;

export struct TableMeta : public MetaInterface {
    using EntryT = TableEntry;

    friend class DBEntry;
    friend struct Catalog;

public:
    inline explicit TableMeta(const SharedPtr<String> &db_entry_dir, const SharedPtr<String> table_name, DBEntry *db_entry)
        : db_entry_dir_(db_entry_dir), table_name_(table_name), db_entry_(db_entry) {}

    static UniquePtr<TableMeta> NewTableMeta(const SharedPtr<String> &db_entry_dir, const SharedPtr<String> &name, DBEntry *db_entry);

    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<TableMeta> Deserialize(const nlohmann::json &table_meta_json, DBEntry *db_entry, BufferManager *buffer_mgr);

    [[nodiscard]] const SharedPtr<String> &table_name_ptr() const { return table_name_; }
    [[nodiscard]] const String &table_name() const { return *table_name_; }
    [[nodiscard]] const SharedPtr<String> &db_name_ptr() const;
    [[nodiscard]] const SharedPtr<String> &db_entry_dir_ptr() const { return db_entry_dir_; }
    [[nodiscard]] const String &db_entry_dir() const { return *db_entry_dir_; }

private:
    Tuple<TableEntry *, Status> CreateEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                            TableEntryType table_entry_type,
                                            const SharedPtr<String> &table_collection_name,
                                            const Vector<SharedPtr<ColumnDef>> &columns,
                                            TransactionID txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnManager *txn_mgr,
                                            ConflictType conflict_type);

    Tuple<SharedPtr<TableEntry>, Status> DropEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnManager *txn_mgr,
                                                   const String &table_name,
                                                   ConflictType conflict_type);

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<TableEntry *, Status> GetEntry(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
        return table_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    }

    Tuple<TableEntry *, Status> GetEntryNolock(TransactionID txn_id, TxnTimeStamp begin_ts) {
        return table_entry_list_.GetEntryNolock(txn_id, begin_ts);
    }

    void DeleteEntry(TransactionID txn_id);

    // replay
    void
    CreateEntryReplay(std::function<SharedPtr<TableEntry>(TransactionID, TxnTimeStamp)> &&init_entry, TransactionID txn_id, TxnTimeStamp begin_ts);

    void DropEntryReplay(std::function<SharedPtr<TableEntry>(TransactionID, TxnTimeStamp)> &&init_entry, TransactionID txn_id, TxnTimeStamp begin_ts);

    // GetEntryReplay(txn_id, begin_ts);
    TableEntry *GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);
    //

private:
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> table_name_{};

    DBEntry *db_entry_{};

private:
    EntryList<TableEntry> table_entry_list_{};

    // TODO: remove
    std::shared_mutex &rw_locker() { return table_entry_list_.rw_locker_; };

    // TODO: remove
    List<SharedPtr<TableEntry>> &table_entry_list() { return table_entry_list_.entry_list_; }

public:
    void Cleanup() override;

    bool PickCleanup(CleanupScanner *scanner) override;

    bool Empty() override { return table_entry_list_.Empty(); }
};

} // namespace infinity
