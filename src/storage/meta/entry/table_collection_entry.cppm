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

import stl;
import parser;
import txn_store;
import buffer_manager;
import third_party;
import table_collection_type;
import base_entry;
import segment_entry;
import block_index;
import table_collection_meta;
import data_access_state;
import txn_manager;
import index_entry;
import segment_entry;
import index_def_meta;
import txn;
import status;

export module table_collection_entry;

namespace infinity {

class DBEntry;
class IndexDef;
class IndexDefEntry;

export struct TableCollectionEntry : public BaseEntry {
public:
    explicit TableCollectionEntry(const SharedPtr<String> &db_entry_dir,
                                  SharedPtr<String> table_collection_name,
                                  const Vector<SharedPtr<ColumnDef>> &columns,
                                  TableCollectionType table_collection_type,
                                  TableCollectionMeta *table_collection_meta,
                                  u64 txn_id,
                                  TxnTimeStamp begin_ts);

public:
    static Status CreateIndex(TableCollectionEntry *table_entry,
                              SharedPtr<IndexDef> index_def,
                              ConflictType conflict_type,
                              u64 txn_id,
                              TxnTimeStamp begin_ts,
                              TxnManager *txn_mgr,
                              BaseEntry *&new_index_entry);

    static Status DropIndex(TableCollectionEntry *table_entry,
                            const String &index_name,
                            ConflictType conflict_type,
                            u64 txn_id,
                            TxnTimeStamp begin_ts,
                            TxnManager *txn_mgr,
                            BaseEntry *&new_index_entry);

    static Status GetIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry *&index_entry);

    static void RemoveIndexEntry(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnManager *txn_mgr);

public:
    static void Append(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store, BufferManager *buffer_mgr);

    static void CreateIndexFile(TableCollectionEntry *table_entry,
                                void *txn_store,
                                IndexDefEntry *index_def_entry,
                                TxnTimeStamp begin_ts,
                                BufferManager *buffer_mgr);

    static UniquePtr<String> Delete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &delete_state);

    static void CommitAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, const AppendState *append_state_ptr);

    static void CommitCreateIndex(TableCollectionEntry *table_entry, HashMap<String, TxnIndexStore> &txn_indexes_store_);

    static void RollbackAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store);

    static void CommitDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, const DeleteState &append_state);

    static UniquePtr<String> RollbackDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr);

    static UniquePtr<String> ImportSegment(TableCollectionEntry *table_entry, Txn *txn_ptr, SharedPtr<SegmentEntry> segment);

    static inline u32 GetNextSegmentID(TableCollectionEntry *table_entry) { return table_entry->next_segment_id_++; }

    static inline u32 GetMaxSegmentID(const TableCollectionEntry *table_entry) { return table_entry->next_segment_id_; }

    static SegmentEntry *GetSegmentByID(const TableCollectionEntry *table_entry, u32 seg_id);

    static DBEntry *GetDBEntry(const TableCollectionEntry *table_entry);

    static SharedPtr<String> GetDBName(const TableCollectionEntry *table_entry);

    inline static TableCollectionMeta *GetTableMeta(const TableCollectionEntry *table_entry) { return table_entry->table_collection_meta_; }

    static SharedPtr<BlockIndex> GetBlockIndex(TableCollectionEntry *table_collection_entry, u64 txn_id, TxnTimeStamp begin_ts);

    static Json Serialize(TableCollectionEntry *table_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<TableCollectionEntry> Deserialize(const Json &table_entry_json, TableCollectionMeta *table_meta, BufferManager *buffer_mgr);

    virtual void MergeFrom(BaseEntry &other);

public:
    u64 GetColumnIdByName(const String &column_name);

private:
    HashMap<String, u64> column_name2column_id_;

public:
    RWMutex rw_locker_{};

    SharedPtr<String> table_entry_dir_{};

    SharedPtr<String> table_collection_name_{};
    Vector<SharedPtr<ColumnDef>> columns_{}; // 2. So this should be HashMap
    TableCollectionType table_collection_type_{TableCollectionType::kTableEntry};

    TableCollectionMeta *table_collection_meta_{};

    // From data table
    SizeT row_count_{};
    HashMap<u32, SharedPtr<SegmentEntry>> segments_{};
    SegmentEntry *unsealed_segment_{};
    atomic_u32 next_segment_id_{};

    // Index definition
    HashMap<String, UniquePtr<IndexDefMeta>> indexes_{};
};

} // namespace infinity
