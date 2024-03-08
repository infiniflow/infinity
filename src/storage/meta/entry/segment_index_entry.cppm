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

export module segment_index_entry;

import stl;
import buffer_handle;
import third_party;
import buffer_obj;
import base_entry;
import index_file_worker;
import status;
import index_base;
import column_def;
import meta_entry_interface;
import cleanup_scanner;
import memory_indexer;

namespace infinity {

class Txn;
class TableIndexEntry;
struct TableEntry;
class FaissIndexPtr;
class BufferManager;
struct SegmentEntry;

export class SegmentIndexEntry : public BaseEntry, public EntryInterface {

public:
    static SharedPtr<SegmentIndexEntry>
    NewIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Txn *txn, CreateIndexParam *create_index_param);

    static SharedPtr<SegmentIndexEntry> NewReplaySegmentIndexEntry(TableIndexEntry *table_index_entry,
                                                                   TableEntry *table_entry,
                                                                   SegmentID segment_id,
                                                                   BufferManager *buffer_manager,
                                                                   TxnTimeStamp min_ts,
                                                                   TxnTimeStamp max_ts,
                                                                   TransactionID txn_id,
                                                                   TxnTimeStamp begin_ts,
                                                                   TxnTimeStamp commit_ts,
                                                                   bool is_delete);

    [[nodiscard]] BufferHandle GetIndex();

    // load the idx part into memory
    [[nodiscard]] BufferHandle GetIndexPartAt(u32 idx);

    [[nodiscard]] inline u32 GetIndexPartNum() { return vector_buffer_.size() - 1; }

    nlohmann::json Serialize();

    void SaveIndexFile();

    static UniquePtr<SegmentIndexEntry>
    Deserialize(const nlohmann::json &index_entry_json, TableIndexEntry *table_index_entry, BufferManager *buffer_mgr, TableEntry *table_entry);

    bool Flush(TxnTimeStamp checkpoint_ts);

    void Cleanup() final;

    void PickCleanup(CleanupScanner *scanner) final;

    TableIndexEntry *table_index_entry() const { return table_index_entry_; }

public:
    // Getter
    inline SegmentID segment_id() const { return segment_id_; }
    inline TxnTimeStamp min_ts() const { return min_ts_; }
    inline TxnTimeStamp max_ts() const { return max_ts_; }

    Status CreateIndexPrepare(const IndexBase *index_base,
                              const ColumnDef *column_def,
                              const SegmentEntry *segment_entry,
                              Txn *txn,
                              bool prepare,
                              bool check_ts);

    Status CreateIndexDo(const IndexBase *index_base, const ColumnDef *column_def, atomic_u64 &create_index_idx);

    static UniquePtr<CreateIndexParam> GetCreateIndexParam(const IndexBase *index_base, SizeT seg_row_count, const ColumnDef *column_def);

private:
    explicit SegmentIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Vector<BufferObj *> vector_buffer);
    void UpdateIndex(TxnTimeStamp commit_ts, FaissIndexPtr *index, BufferManager *buffer_mgr);
    // Load from disk. Is called by SegmentIndexEntry::Deserialize.
    static UniquePtr<SegmentIndexEntry>
    LoadIndexEntry(TableIndexEntry *table_index_entry, u32 segment_id, BufferManager *buffer_manager, CreateIndexParam *create_index_param);

private:
    TableIndexEntry *table_index_entry_;
    const SegmentID segment_id_{};

    Vector<BufferObj *> vector_buffer_{}; // size: 1 + GetIndexPartNum().

    std::shared_mutex rw_locker_{};

    TxnTimeStamp min_ts_{0}; // Indicate the commit_ts which create this SegmentIndexEntry
    TxnTimeStamp max_ts_{0}; // Indicate the max commit_ts which update data inside this SegmentIndexEntry
    TxnTimeStamp checkpoint_ts_{0};

    UniquePtr<MemoryIndexer> memory_indexer_{};
};

} // namespace infinity
