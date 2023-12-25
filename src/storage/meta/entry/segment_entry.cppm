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
import default_values;
import base_entry;
import block_entry;
import third_party;
import txn;
import buffer_manager;
import index_base;
import data_access_state;
import parser;
import segment_column_index_entry;
import table_index_entry;
import txn_store;
import index_file_worker;
import column_index_entry;

export module segment_entry;

namespace infinity {

class TableCollectionEntry;

export struct SegmentEntry;

struct SegmentEntry : public BaseEntry {
public:
    explicit SegmentEntry(const TableCollectionEntry *table_entry);

    RWMutex rw_locker_{};

    const TableCollectionEntry *table_entry_{};

    SharedPtr<String> segment_dir_{};
    u32 segment_id_{};

    SizeT row_count_{};
    SizeT row_capacity_{};
    u64 column_count_{};

    TxnTimeStamp min_row_ts_{0}; // Indicate the commit_ts which create this SegmentEntry
    TxnTimeStamp max_row_ts_{0}; // Indicate the max commit_ts which create/update/delete data inside this SegmentEntry

    Vector<SharedPtr<BlockEntry>> block_entries_{};

public:
    static SharedPtr<SegmentEntry> MakeNewSegmentEntry(const TableCollectionEntry *table_entry, u32 segment_id, BufferManager *buffer_mgr);

    static SharedPtr<SegmentEntry>
    MakeReplaySegmentEntry(const TableCollectionEntry *table_entry, u32 segment_id, SharedPtr<String> segment_dir, TxnTimeStamp commit_ts);

    static u64 AppendData(SegmentEntry *segment_entry, Txn *txn_ptr, AppendState *append_state_ptr, BufferManager *buffer_mgr);

    static void DeleteData(SegmentEntry *segment_entry, Txn *txn_ptr, const HashMap<u16, Vector<RowID>> &block_row_hashmap);

    static SharedPtr<SegmentColumnIndexEntry> CreateIndexFile(SegmentEntry *segment_entry,
                                                              ColumnIndexEntry *column_index_entry,
                                                              SharedPtr<ColumnDef> column_def,
                                                              TxnTimeStamp create_ts,
                                                              BufferManager *buffer_mgr,
                                                              TxnTableStore *txn_store);

    static SharedPtr<SegmentColumnIndexEntry> CreateIndexFilePrepare(SegmentEntry *segment_entry,
                                                                     ColumnIndexEntry *column_index_entry,
                                                                     SharedPtr<ColumnDef> column_def,
                                                                     TxnTimeStamp create_ts,
                                                                     BufferManager *buffer_mgr);

    static void CommitAppend(SegmentEntry *segment_entry, Txn *txn_ptr, u16 block_id, u16 start_pos, u16 row_count);

    static void CommitDelete(SegmentEntry *segment_entry, Txn *txn_ptr, const HashMap<u16, Vector<RowID>> &block_row_hashmap);

    static u16 GetMaxBlockID(const SegmentEntry *segment_entry);

    static BlockEntry *GetBlockEntryByID(const SegmentEntry *segment_entry, u16 block_id);

    static Json Serialize(SegmentEntry *segment_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static SharedPtr<SegmentEntry> Deserialize(const Json &table_entry_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr);

    static int Room(SegmentEntry *segment_entry);

    void MergeFrom(infinity::BaseEntry &other) override;

    const String &DirPath() { return *segment_dir_; }

private:
    static SharedPtr<String> DetermineSegmentDir(const String &parent_dir, u32 seg_id);

public:
    static UniquePtr<CreateIndexParam>
    GetCreateIndexParam(const SegmentEntry *segment_entry, const IndexBase* index_base, const ColumnDef* column_def);
};

} // namespace infinity
