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
import base_entry;
import parser;
import txn_manager;
import third_party;
import index_def;
import base_entry;
import status;

export module table_index_meta;

namespace infinity {

class BufferManager;
class TableCollectionEntry;
class SegmentEntry;

export class TableIndexMeta {
public:
    explicit TableIndexMeta(TableCollectionEntry *table_collection_entry, SharedPtr<String> index_name);

public:
    static Status CreateTableIndexEntry(TableIndexMeta *table_index_meta,
                                        const SharedPtr<IndexDef> &index_def,
                                        ConflictType conflict_type,
                                        u64 txn_id,
                                        TxnTimeStamp begin_ts,
                                        TxnManager *txn_mgr,
                                        BaseEntry *&new_index_entry);

    static Status DropTableIndexEntry(TableIndexMeta *table_index_meta,
                                      ConflictType conflict_type,
                                      u64 txn_id,
                                      TxnTimeStamp begin_ts,
                                      TxnManager *txn_mgr,
                                      BaseEntry *&new_index_entry);

    static SharedPtr<String> ToString(TableIndexMeta *table_index_meta);

    static inline TableCollectionEntry *GetTableCollectionEntry(TableIndexMeta *table_index_meta) {
        return table_index_meta->table_collection_entry_;
    }

    static Json Serialize(TableIndexMeta *table_index_meta, TxnTimeStamp max_commit_ts);

    static UniquePtr<TableIndexMeta> Deserialize(const Json &index_def_meta_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr);

    static Status GetEntry(TableIndexMeta *meta, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry *&base_entry);

    static void DeleteNewEntry(TableIndexMeta *meta, u64 txn_id, TxnManager *txn_mgr);


    void MergeFrom(TableIndexMeta &other);

private:
    static Status CreateTableIndexEntryInternal(TableIndexMeta *table_index_meta,
                                                const SharedPtr<IndexDef> &index_def,
                                                u64 txn_id,
                                                TxnTimeStamp begin_ts,
                                                TxnManager *txn_mgr,
                                                BaseEntry *&new_index_entry);

    static Status DropTableIndexEntryInternal(TableIndexMeta *table_index_meta,
                                              u64 txn_id,
                                              TxnTimeStamp begin_ts,
                                              TxnManager *txn_mgr,
                                              BaseEntry *&new_index_entry);

private:
    //    RWMutex rw_locker_{};
    SharedPtr<String> index_name_{};
    TableCollectionEntry *table_collection_entry_{};

    RWMutex rw_locker_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};
} // namespace infinity