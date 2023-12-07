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

import base_entry;
import stl;
import table_collection_type;
import table_collection_entry;
import parser;
import third_party;
import table_collection_detail;
import table_collection_meta;
import buffer_manager;
import txn_manager;
import status;

export module db_entry;

namespace infinity {

export class DBEntry : public BaseEntry {
public:
    inline explicit DBEntry(const SharedPtr<String> &data_dir, SharedPtr<String> db_name, u64 txn_id, TxnTimeStamp begin_ts)
        : BaseEntry(EntryType::kDatabase), db_entry_dir_(MakeShared<String>(Format("{}/{}/txn_{}", *data_dir, *db_name, txn_id))),
          db_name_(Move(db_name)) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

public:
    static Status CreateTableCollection(DBEntry *db_entry,
                                        TableCollectionType table_collection_type,
                                        const SharedPtr<String> &table_collection_name,
                                        const Vector<SharedPtr<ColumnDef>> &columns,
                                        u64 txn_id,
                                        TxnTimeStamp begin_ts,
                                        TxnManager *txn_mgr,
                                        BaseEntry *&base_entry);

    static Status DropTableCollection(DBEntry *db_entry,
                                      const String &table_collection_name,
                                      ConflictType conflict_type,
                                      u64 txn_id,
                                      TxnTimeStamp begin_ts,
                                      TxnManager *txn_mgr,
                                      BaseEntry *&base_entry);

    static Tuple<BaseEntry*, Status>
    GetTableCollection(DBEntry *db_entry, const String &table_collection_name, u64 txn_id, TxnTimeStamp begin_ts);

    static void RemoveTableCollectionEntry(DBEntry *db_entry, const String &table_collection_name, u64 txn_id, TxnManager *txn_mgr);

    static Vector<TableCollectionEntry *> TableCollections(DBEntry *db_entry, u64 txn_id, TxnTimeStamp begin_ts);

    static Status GetTableCollectionsDetail(DBEntry *db_entry, u64 txn_id, TxnTimeStamp begin_ts, Vector<TableCollectionDetail> &output_table_array);

    static SharedPtr<String> ToString(DBEntry *db_entry);

    static Json Serialize(DBEntry *db_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<DBEntry> Deserialize(const Json &db_entry_json, BufferManager *buffer_mgr);

    virtual void MergeFrom(BaseEntry &other);

public:
    RWMutex rw_locker_{};
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> db_name_{};
    HashMap<String, UniquePtr<TableCollectionMeta>> tables_{}; // NOTE : can use SharedPtr<String> as key.
};

} // namespace infinity
