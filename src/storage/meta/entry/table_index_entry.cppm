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

export module table_index_entry;

import stl;

import txn_store;
import fulltext_index_entry;
import column_index_entry;
import segment_column_index_entry;
import base_entry;
import index_def;
import block_index;
import third_party;
import status;

namespace infinity {

class Txn;
class TableIndexMeta;
class BufferManager;
struct TableEntry;
class BaseTableRef;

export struct TableIndexEntry : public BaseEntry {

    friend struct TableEntry;

public:
    TableIndexEntry(const SharedPtr<IndexDef> &index_def,
                    TableIndexMeta *table_index_meta,
                    SharedPtr<String> index_dir,
                    TransactionID txn_id,
                    TxnTimeStamp begin_ts,
                    bool replay = false);

    TableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<TableIndexEntry> NewTableIndexEntry(const SharedPtr<IndexDef> &index_def,
                                                         TableIndexMeta *table_index_meta,
                                                         Txn *txn,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         bool is_replay = false,
                                                         String replay_table_index_dir = "");

    static SharedPtr<TableIndexEntry> NewDropTableIndexEntry(TableIndexMeta *table_index_meta, TransactionID txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<TableIndexEntry> NewReplayTableIndexEntry(TableIndexMeta *table_index_meta,
                                                               const SharedPtr<IndexDef> &index_def,
                                                               const SharedPtr<String> &index_dir,
                                                               TransactionID txn_id,
                                                               TxnTimeStamp begin_ts,
                                                               TxnTimeStamp commit_ts,
                                                               bool is_delete);

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    static SharedPtr<TableIndexEntry>
    Deserialize(const nlohmann::json &index_def_entry_json, TableIndexMeta *table_index_meta, BufferManager *buffer_mgr, TableEntry *table_entry);

public:
    // Getter
    inline const TableIndexMeta *table_index_meta() const { return table_index_meta_; }
    inline const IndexDef *index_def() const { return index_def_.get(); }
    const SharedPtr<IndexDef> &table_index_def() { return index_def_; }
    SharedPtr<FulltextIndexEntry> &fulltext_index_entry() { return fulltext_index_entry_; }
    HashMap<u64, SharedPtr<ColumnIndexEntry>> &column_index_map() { return column_index_map_; }
    SharedPtr<String> index_dir() { return index_dir_; }

    Status CreateIndexPrepare(TableEntry *table_entry, BlockIndex *block_index, Txn *txn, bool prepare, bool is_replay, bool check_ts = true);

    Status CreateIndexDo(const TableEntry *table_entry, HashMap<SegmentID, atomic_u64> &create_index_idxes);

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

    // For SegmentColumnIndexEntry
    void CommitCreateIndex(u64 column_id, u32 segment_id, SharedPtr<SegmentColumnIndexEntry> index_entry, bool is_replay = false);
    // For FulltextIndexEntry
    void CommitCreateIndex(const SharedPtr<FulltextIndexEntry> &fulltext_index_entry);

private:
    std::shared_mutex rw_locker_{};
    TableIndexMeta *table_index_meta_{};
    const SharedPtr<IndexDef> index_def_{};
    SharedPtr<String> index_dir_{};

    HashMap<ColumnID, SharedPtr<ColumnIndexEntry>> column_index_map_{};

    SharedPtr<FulltextIndexEntry> fulltext_index_entry_{};
};

} // namespace infinity
