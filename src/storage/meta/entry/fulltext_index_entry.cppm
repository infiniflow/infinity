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

export module fulltext_index_entry;

import stl;
import index_base;
import third_party;
import base_entry;

namespace infinity {

class Txn;
struct TableIndexEntry;
class BufferManager;
struct TableEntry;
class IRSDataStore;
class IndexFullText;

export struct FulltextIndexEntry : public BaseEntry {
public:
    FulltextIndexEntry(TableIndexEntry *table_index_entry, bool deleted, TransactionID txn_id, TxnTimeStamp begin_ts);
    ~FulltextIndexEntry() override = default;

    static SharedPtr<FulltextIndexEntry> NewFulltextIndexEntry(TableIndexEntry *table_index_entry, TransactionID txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<FulltextIndexEntry> NewReplayFulltextIndexEntry(TableIndexEntry *table_index_entry,
                                                                     TransactionID txn_id,
                                                                     TxnTimeStamp begin_ts,
                                                                     TxnTimeStamp commit_ts,
                                                                     bool is_delete);

    void AddColumn(SharedPtr<IndexBase> index_base, ColumnID column_id);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static SharedPtr<FulltextIndexEntry>
    Deserialize(const nlohmann::json &index_def_entry_json, TableIndexEntry *table_index_entry, BufferManager *buffer_mgr);

    void Cleanup();

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

public:
    const TableIndexEntry *table_index_entry() { return table_index_entry_; }
    const SharedPtr<String> &index_dir() const;
    HashMap<u64, SharedPtr<IndexFullText>> &index_info_map() { return index_info_map_; }

public:
    std::shared_mutex rw_locker_{};

    const TableIndexEntry *table_index_entry_{};
    HashMap<u64, SharedPtr<IndexFullText>> index_info_map_{};
    UniquePtr<IRSDataStore> irs_index_{};
};
} // namespace infinity
