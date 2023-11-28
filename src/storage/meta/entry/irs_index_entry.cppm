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
import index_base;
import base_entry;
import third_party;

export module irs_index_entry;

namespace infinity {

class TableIndexEntry;
class BufferManager;
class TableCollectionEntry;
class IRSDataStore;

export struct IrsIndexEntry : public BaseEntry {
public:
    IrsIndexEntry(TableIndexEntry *table_index_entry, SharedPtr<String> index_dir, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<IrsIndexEntry> NewIrsIndexEntry(HashMap<u64, SharedPtr<IndexBase>> index_info_map,
                                                     TableIndexEntry *table_index_entry,
                                                     u64 txn_id,
                                                     SharedPtr<String> index_dir,
                                                     TxnTimeStamp begin_ts);

    void AddColumn(SharedPtr<IndexBase> index_base, u64 column_id);

    static Json Serialize(const IrsIndexEntry *index_def_entry, TxnTimeStamp max_commit_ts);

    static SharedPtr<IrsIndexEntry> Deserialize(const Json &index_def_entry_json, TableIndexEntry *table_index_entry, BufferManager *buffer_mgr);

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

public:
    RWMutex rw_locker_{};

    const TableIndexEntry *table_index_entry_{};
    SharedPtr<String> index_dir_{};
    HashMap<u64, SharedPtr<IndexBase>> index_info_map_{};
    SharedPtr<IRSDataStore> irs_index_{};
};
} // namespace infinity
