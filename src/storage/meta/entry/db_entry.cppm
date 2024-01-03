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

export module catalog:db_entry;

import :table_meta;
import :base_entry;

import stl;
import table_entry_type;
import parser;
import third_party;
import table_detail;
import buffer_manager;
import status;

namespace infinity {

class DBEntry : public BaseEntry {
    friend struct NewCatalog;

public:
    inline explicit DBEntry(const SharedPtr<String> &data_dir, SharedPtr<String> db_name, u64 txn_id, TxnTimeStamp begin_ts)
        : BaseEntry(EntryType::kDatabase), db_entry_dir_(MakeShared<String>(Format("{}/{}/txn_{}", *data_dir, *db_name, txn_id))),
          db_name_(Move(db_name)) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

public:

    static SharedPtr<String> ToString(DBEntry *db_entry);

    static Json Serialize(DBEntry *db_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<DBEntry> Deserialize(const Json &db_entry_json, BufferManager *buffer_mgr);

    virtual void MergeFrom(BaseEntry &other);

    [[nodiscard]] const String& db_name() const { return *db_name_; }

    [[nodiscard]] const SharedPtr<String>& db_name_ptr() const { return db_name_; }

private:
    Tuple<TableEntry *, Status> CreateTable(TableEntryType table_entry_type,
                                            const SharedPtr<String> &table_collection_name,
                                            const Vector<SharedPtr<ColumnDef>> &columns,
                                            u64 txn_id,
                                            TxnTimeStamp begin_ts,
                                            TxnManager *txn_mgr);

    Tuple<TableEntry *, Status>
    DropTable(const String &table_collection_name, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    Tuple<TableEntry *, Status> GetTableCollection(const String &table_name, u64 txn_id, TxnTimeStamp begin_ts);

    void RemoveTableEntry(const String &table_collection_name, u64 txn_id, TxnManager *txn_mgr);

    Vector<TableEntry *> TableCollections(u64 txn_id, TxnTimeStamp begin_ts);

    Status GetTablesDetail(u64 txn_id, TxnTimeStamp begin_ts, Vector<TableDetail> &output_table_array);

private:
    RWMutex rw_locker_{};
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> db_name_{};
    HashMap<String, UniquePtr<TableMeta>> tables_{}; // NOTE : can use SharedPtr<String> as key.
};

} // namespace infinity
