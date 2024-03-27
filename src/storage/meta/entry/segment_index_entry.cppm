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
import internal_types;
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
import chunk_index_entry;
import memory_indexer;

namespace infinity {

class Txn;
struct TableIndexEntry;
struct BlockEntry;
struct TableEntry;
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

    // MemIndexInsert is non-blocking. Caller must ensure there's no RowID gap between each call.
    void MemIndexInsert(Txn *txn, SharedPtr<BlockEntry> block_entry, u32 row_offset, u32 row_count);

    // User shall invoke this reguarly to populate recently inserted rows into the fulltext index. Noop for other types of index.
    void MemIndexCommit();

    // Dump or spill the memory indexer
    void MemIndexDump(bool spill = false);

    // Init the mem index from previously spilled one.
    void MemIndexLoad(const String &base_name, RowID base_row_id);

    // Populate index entirely for the segment
    void PopulateEntirely(const SegmentEntry *segment_entry, Txn *txn);

    Status CreateIndexPrepare(const SegmentEntry *segment_entry, Txn *txn, bool prepare, bool check_ts);

    Status CreateIndexDo(atomic_u64 &create_index_idx);

    static UniquePtr<CreateIndexParam> GetCreateIndexParam(SharedPtr<IndexBase> index_base, SizeT seg_row_count, SharedPtr<ColumnDef> column_def);

    Tuple<Vector<String>, Vector<RowID>, MemoryIndexer *> GetFullTextIndexSnapshot() {
        Vector<String> base_names;
        Vector<RowID> base_rowids;
        std::shared_lock lock(rw_locker_);
        for (SizeT i = 0; i < chunk_index_entries_.size(); i++) {
            auto &chunk_index_entry = chunk_index_entries_[i];
            base_names.push_back(chunk_index_entry->base_name_);
            base_rowids.push_back(chunk_index_entry->base_rowid_);
        }
        return {base_names, base_rowids, memory_indexer_.get()};
    }
    Pair<u64, u32> GetFulltextColumnLenInfo() {
        std::shared_lock lock(rw_locker_);
        return {ft_column_len_sum_, ft_column_len_cnt_};
    }
    void UpdateFulltextColumnLenInfo(u64 column_len_sum, u32 column_len_cnt) {
        std::unique_lock lock(rw_locker_);
        ft_column_len_sum_ += column_len_sum;
        ft_column_len_cnt_ += column_len_cnt;
    }

public:
    void AddChunkIndexEntry(const String &base_name, RowID base_rowid, u32 row_count);
    // only for unittest
    MemoryIndexer *GetMemoryIndexer() { return memory_indexer_.get(); }
    void SetMemoryIndexer(UniquePtr<MemoryIndexer> &&memory_indexer) { memory_indexer_ = std::move(memory_indexer); }
    static SharedPtr<SegmentIndexEntry> CreateFakeEntry();

private:
    explicit SegmentIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Vector<BufferObj *> vector_buffer);
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

    Vector<UniquePtr<ChunkIndexEntry>> chunk_index_entries_{};
    UniquePtr<MemoryIndexer> memory_indexer_{};

    u64 ft_column_len_sum_{}; // increase only
    u32 ft_column_len_cnt_{}; // increase only
};

} // namespace infinity
