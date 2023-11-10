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
import base_meta;

export module index_def_meta;

namespace infinity {

class TableCollectionEntry;

export class IndexDefMeta : public BaseMeta{
public:
    explicit IndexDefMeta(SharedPtr<String> index_name, TableCollectionEntry *table_collection_entry);

public:
    static EntryResult CreateNewEntry(IndexDefMeta *index_def_meta,
                                      SharedPtr<IndexDef> index_def,
                                      ConflictType conflict_type,
                                      u64 txn_id,
                                      TxnTimeStamp begin_ts,
                                      TxnManager *txn_mgr);

    static SharedPtr<String> ToString(IndexDefMeta *index_def_meta);

    static inline TableCollectionEntry *GetTableCollectionEntry(IndexDefMeta *index_def_meta) { return index_def_meta->table_collection_entry_; }

    static Json Serialize(const IndexDefMeta *index_def_meta);

    static UniquePtr<IndexDefMeta> Deserialize(const Json &index_def_meta_json, TableCollectionEntry *table_collection_entry);

public:
    RWMutex rw_locker_{};

    TableCollectionEntry *table_collection_entry_{};

    SharedPtr<String> index_name_{};
    List<UniquePtr<BaseEntry>> entry_list_{};
};
} // namespace infinity