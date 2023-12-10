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
import parser;
import third_party;
import table_collection_type;
import txn_manager;
import buffer_manager;
import status;

export module table_collection_meta;

namespace infinity {

class DBEntry;

export struct TableCollectionMeta {
public:
    inline explicit TableCollectionMeta(const SharedPtr<String> &db_entry_dir, SharedPtr<String> name, DBEntry *db_entry)
        : db_entry_dir_(db_entry_dir), table_collection_name_(Move(name)), db_entry_(db_entry) {}

public:
    static Status CreateNewEntry(TableCollectionMeta *table_meta,
                                 TableCollectionType table_collection_type,
                                 const SharedPtr<String> &table_collection_name,
                                 const Vector<SharedPtr<ColumnDef>> &columns,
                                 u64 txn_id,
                                 TxnTimeStamp begin_ts,
                                 TxnManager *txn_mgr,
                                 BaseEntry *&base_entry);

    static Status DropNewEntry(TableCollectionMeta *table_meta,
                               u64 txn_id,
                               TxnTimeStamp begin_ts,
                               TxnManager *txn_mgr,
                               const String &table_name,
                               ConflictType conflict_type,
                               BaseEntry *&base_entry);

    static void DeleteNewEntry(TableCollectionMeta *table_meta, u64 txn_id, TxnManager *txn_mgr);

    static Tuple<BaseEntry*, Status> GetEntry(TableCollectionMeta *table_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<String> ToString(TableCollectionMeta *table_meta);

    static inline DBEntry *GetDBEntry(TableCollectionMeta *table_meta) { return table_meta->db_entry_; }

    static Json Serialize(TableCollectionMeta *table_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<TableCollectionMeta> Deserialize(const Json &table_meta_json, DBEntry *db_entry, BufferManager *buffer_mgr);

    void MergeFrom(TableCollectionMeta &other);

public:
    RWMutex rw_locker_{};
    SharedPtr<String> db_entry_dir_{};
    SharedPtr<String> table_collection_name_{};

    DBEntry *db_entry_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

} // namespace infinity
