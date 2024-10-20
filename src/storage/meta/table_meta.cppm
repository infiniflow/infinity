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
import status;
import extra_ddl_info;
import column_def;
import base_entry;

import table_entry;
import entry_list;
import meta_info;
import cleanup_scanner;

namespace infinity {

struct DBEntry;
class TxnManager;
class Txn;

export struct TableMeta : public BaseMeta {
    using EntryT = TableEntry;

    friend struct DBEntry;
    friend struct Catalog;

public:
    inline explicit TableMeta(const SharedPtr<String> &db_entry_dir, const SharedPtr<String> table_name, DBEntry *db_entry)
        : db_entry_dir_(db_entry_dir), table_name_(table_name), db_entry_(db_entry) {}

    static UniquePtr<TableMeta> NewTableMeta(const SharedPtr<String> &db_entry_dir, const SharedPtr<String> &name, DBEntry *db_entry);

    static UniquePtr<TableMeta>
    NewTableMeta(const SharedPtr<String> &data_dir, const SharedPtr<String> &db_entry_dir, const SharedPtr<String> &name, DBEntry *db_entry);

    SharedPtr<String> ToString();

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static UniquePtr<TableMeta> Deserialize(const nlohmann::json &table_meta_json, DBEntry *db_entry);

    [[nodiscard]] const SharedPtr<String> &table_name_ptr() const { return table_name_; }
    [[nodiscard]] const String &table_name() const { return *table_name_; }
    [[nodiscard]] const SharedPtr<String> &db_name_ptr() const;

    // Relative to the `data_dir`config item
    [[nodiscard]] const SharedPtr<String> &db_entry_dir() const { return db_entry_dir_; }

    DBEntry *db_entry() { return db_entry_; }

    List<SharedPtr<TableEntry>> GetAllEntries() const {
        return table_entry_list_.GetAllEntries();
    }
private:
    Tuple<TableEntry *, Status> CreateEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                            TableEntryType table_entry_type,
                                            const SharedPtr<String> &table_name,
                                            const SharedPtr<String> &table_comment,
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

    Tuple<SharedPtr<TableInfo>, Status> GetTableInfo(std::shared_lock<std::shared_mutex> &&r_lock, Txn *txn);

    Tuple<TableEntry *, Status> GetEntry(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
        return table_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    }

    Tuple<TableEntry *, Status> GetEntryNolock(TransactionID txn_id, TxnTimeStamp begin_ts) {
        return table_entry_list_.GetEntryNolock(txn_id, begin_ts);
    }

    Status AddEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                    SharedPtr<TableEntry> table_entry,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    TxnManager *txn_mgr,
                    bool add_if_exist = false);

    void DeleteEntry(TransactionID txn_id);

    // replay
    void
    CreateEntryReplay(std::function<SharedPtr<TableEntry>(TransactionID, TxnTimeStamp)> &&init_entry, TransactionID txn_id, TxnTimeStamp begin_ts);

    void UpdateEntryReplay(std::function<void(SharedPtr<TableEntry>, TransactionID, TxnTimeStamp)> &&update_entry,
                           TransactionID txn_id,
                           TxnTimeStamp begin_ts);

    void DropEntryReplay(std::function<SharedPtr<TableEntry>(TransactionID, TxnTimeStamp)> &&init_entry, TransactionID txn_id, TxnTimeStamp begin_ts);

    TableEntry *GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);

    void PushBackEntry(const SharedPtr<TableEntry>& new_table_entry);

    void Sort();

private:
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> table_name_{};

    DBEntry *db_entry_{};

private:
    EntryList<TableEntry> table_entry_list_{};

public:
    void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true) override;

    bool PickCleanup(CleanupScanner *scanner) override;

    bool Empty() override { return table_entry_list_.Empty(); }
};

} // namespace infinity
