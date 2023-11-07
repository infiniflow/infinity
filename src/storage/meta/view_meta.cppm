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

export module view_meta;

namespace infinity {

class TxnManager;

class DBEntry;

export struct ViewMeta {
public:
    explicit ViewMeta(SharedPtr<String> name, DBEntry *db_entry) : view_name_(Move(name)), db_entry_(db_entry) {}

public:
    static EntryResult CreateNewEntry(ViewMeta *table_meta,
                                      const SharedPtr<String> &view_name,
                                      const Vector<SharedPtr<ColumnDef>> &columns,
                                      u64 txn_id,
                                      TxnTimeStamp begin_ts,
                                      TxnManager *txn_mgr);

    static EntryResult DropNewEntry(ViewMeta *table_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, const String &table_name);

    static void DeleteNewEntry(ViewMeta *table_meta, u64 txn_id, TxnManager *txn_mgr);

    static EntryResult GetEntry(ViewMeta *table_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<String> ToString(ViewMeta *table_meta);

    static inline void *GetDBEntry(ViewMeta *view_meta) { return view_meta->db_entry_; }

public:
    RWMutex rw_locker_{};
    SharedPtr<String> view_name_{};

    DBEntry *db_entry_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

} // namespace infinity
