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

#include <cassert>

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
import default_values;

namespace infinity {

class Txn;
class TxnTableStore;
struct TableIndexEntry;
struct BlockEntry;
struct TableEntry;
class BufferManager;
struct SegmentEntry;
struct TableEntry;
class SecondaryIndexInMem;
class EMVBIndexInMem;

export struct PopulateEntireConfig {
    bool prepare_;
    bool check_ts_;
};

export class SegmentIndexEntry : public BaseEntry, public EntryInterface {
public:
    static Vector<std::string_view> DecodeIndex(std::string_view encode);

    static String EncodeIndex(const SegmentID segment_id, const TableIndexEntry *table_index_entry);

public:
    static SharedPtr<SegmentIndexEntry>
    NewIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Txn *txn, CreateIndexParam *create_index_param);

    static SharedPtr<SegmentIndexEntry> NewReplaySegmentIndexEntry(TableIndexEntry *table_index_entry,
                                                                   TableEntry *table_entry,
                                                                   SegmentID segment_id,
                                                                   BufferManager *buffer_manager,
                                                                   TxnTimeStamp min_ts,
                                                                   TxnTimeStamp max_ts,
                                                                   u32 next_chunk_id,
                                                                   TransactionID txn_id,
                                                                   TxnTimeStamp begin_ts,
                                                                   TxnTimeStamp commit_ts);

    static Vector<UniquePtr<IndexFileWorker>> CreateFileWorkers(SharedPtr<String> index_dir, CreateIndexParam *param, SegmentID segment_id);

    static String IndexFileName(SegmentID segment_id);

    [[nodiscard]] BufferHandle GetIndex();

    // load the idx part into memory
    [[nodiscard]] BufferHandle GetIndexPartAt(u32 idx);

    [[nodiscard]] inline u32 GetIndexPartNum() { return vector_buffer_.size() - 1; }

    nlohmann::json Serialize(TxnTimeStamp max_commit_ts);

    void SaveIndexFile();

    static UniquePtr<SegmentIndexEntry>
    Deserialize(const nlohmann::json &index_entry_json, TableIndexEntry *table_index_entry, BufferManager *buffer_mgr, TableEntry *table_entry);

    void CommitSegmentIndex(TransactionID txn_id, TxnTimeStamp commit_ts);

    void CommitOptimize(ChunkIndexEntry *new_chunk, const Vector<ChunkIndexEntry *> &old_chunks, TxnTimeStamp commit_ts);

    bool Flush(TxnTimeStamp checkpoint_ts);

    void Cleanup() final;

    void PickCleanup(CleanupScanner *scanner) final;

    TableIndexEntry *table_index_entry() const { return table_index_entry_; }

public:
    // Getter
    inline SegmentID segment_id() const { return segment_id_; }
    inline TxnTimeStamp min_ts() const { return min_ts_; }
    inline TxnTimeStamp max_ts() const { return max_ts_; }
    inline ChunkID next_chunk_id() const { return next_chunk_id_; }
    SharedPtr<String> index_dir() const { return index_dir_; }

    // MemIndexInsert is non-blocking. Caller must ensure there's no RowID gap between each call.
    void MemIndexInsert(SharedPtr<BlockEntry> block_entry, u32 row_offset, u32 row_count, TxnTimeStamp commit_ts, BufferManager *buffer_manager);

    // User shall invoke this reguarly to populate recently inserted rows into the fulltext index. Noop for other types of index.
    void MemIndexCommit();

    // Dump or spill the memory indexer
    SharedPtr<ChunkIndexEntry> MemIndexDump(bool spill = false);

    // Init the mem index from previously spilled one.
    void MemIndexLoad(const String &base_name, RowID base_row_id);

    // Populate index entirely for the segment
    void PopulateEntirely(const SegmentEntry *segment_entry, Txn *txn, const PopulateEntireConfig &config);

    u32 MemIndexRowCount();

    Status CreateIndexPrepare(const SegmentEntry *segment_entry, Txn *txn, bool prepare, bool check_ts);

    Status CreateIndexDo(atomic_u64 &create_index_idx);

    static UniquePtr<CreateIndexParam> GetCreateIndexParam(SharedPtr<IndexBase> index_base, SizeT seg_row_count, SharedPtr<ColumnDef> column_def);

    void GetChunkIndexEntries(Vector<SharedPtr<ChunkIndexEntry>> &chunk_index_entries, TxnTimeStamp begin_ts = MAX_TIMESTAMP) {
        std::shared_lock lock(rw_locker_);
        chunk_index_entries.clear();
        SizeT num = chunk_index_entries_.size();
        for (SizeT i = 0; i < num; i++) {
            auto &chunk_index_entry = chunk_index_entries_[i];
            if (chunk_index_entry->CheckVisibleByTS(begin_ts)) {
                chunk_index_entries.push_back(chunk_index_entry);
            }
        }
        std::sort(std::begin(chunk_index_entries),
                  std::end(chunk_index_entries),
                  [](const SharedPtr<ChunkIndexEntry> &lhs, const SharedPtr<ChunkIndexEntry> &rhs) noexcept {
                      return (lhs->base_rowid_ < rhs->base_rowid_ || (lhs->base_rowid_ == rhs->base_rowid_ && lhs->row_count_ < rhs->row_count_));
                  });
    }

    void RemoveChunkIndexEntry(ChunkIndexEntry *chunk_index_entry) {
        RowID base_rowid = chunk_index_entry->base_rowid_;
        std::unique_lock lock(rw_locker_);
        chunk_index_entries_.erase(std::remove_if(chunk_index_entries_.begin(),
                                                  chunk_index_entries_.end(),
                                                  [base_rowid](const SharedPtr<ChunkIndexEntry> &entry) { return entry->base_rowid_ == base_rowid; }),
                                   chunk_index_entries_.end());
    }

    void ReplaceChunkIndexEntries(TxnTableStore *txn_table_store,
                                  SharedPtr<ChunkIndexEntry> merged_chunk_index_entry,
                                  Vector<ChunkIndexEntry *> &&old_chunks);

    ChunkIndexEntry *RebuildChunkIndexEntries(TxnTableStore *txn_table_store, SegmentEntry *segment_entry);

    Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<MemoryIndexer>> GetFullTextIndexSnapshot() {
        std::shared_lock lock(rw_locker_);
        return {chunk_index_entries_, memory_indexer_};
    }

    Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<ChunkIndexEntry>> GetHnswIndexSnapshot() {
        std::shared_lock lock(rw_locker_);
        return {chunk_index_entries_, memory_hnsw_indexer_};
    }

    Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<ChunkIndexEntry>> GetBMPIndexSnapshot() {
        std::shared_lock lock(rw_locker_);
        return {chunk_index_entries_, memory_hnsw_indexer_};
    }

    Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<SecondaryIndexInMem>> GetSecondaryIndexSnapshot() {
        std::shared_lock lock(rw_locker_);
        return {chunk_index_entries_, memory_secondary_index_};
    }

    Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<EMVBIndexInMem>> GetEMVBIndexSnapshot() {
        std::shared_lock lock(rw_locker_);
        return {chunk_index_entries_, memory_emvb_index_};
    }

    Pair<u64, u32> GetFulltextColumnLenInfo() {
        std::shared_lock lock(rw_locker_);
        if (ft_column_len_sum_ == 0 && memory_indexer_.get() != nullptr) {
            return {memory_indexer_->GetColumnLengthSum(), memory_indexer_->GetDocCount()};
        }
        return {ft_column_len_sum_, ft_column_len_cnt_};
    }
    void UpdateFulltextColumnLenInfo(u64 column_len_sum, u32 column_len_cnt) {
        std::unique_lock lock(rw_locker_);
        ft_column_len_sum_ += column_len_sum;
        ft_column_len_cnt_ += column_len_cnt;
    }

public:
    SharedPtr<ChunkIndexEntry> CreateChunkIndexEntry(SharedPtr<ColumnDef> column_def, RowID base_rowid, BufferManager *buffer_mgr);

    SharedPtr<ChunkIndexEntry> CreateSecondaryIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr);

    SharedPtr<ChunkIndexEntry> CreateEMVBIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr);

    void AddChunkIndexEntry(SharedPtr<ChunkIndexEntry> chunk_index_entry);

    SharedPtr<ChunkIndexEntry> AddFtChunkIndexEntry(const String &base_name, RowID base_rowid, u32 row_count);

    SharedPtr<ChunkIndexEntry> AddChunkIndexEntryReplay(ChunkID chunk_id,
                                                        TableEntry *table_entry,
                                                        const String &base_name,
                                                        RowID base_rowid,
                                                        u32 row_count,
                                                        TxnTimeStamp commit_ts,
                                                        TxnTimeStamp deprecate_ts,
                                                        BufferManager *buffer_mgr);

    // only for unittest
    MemoryIndexer *GetMemoryIndexer() { return memory_indexer_.get(); }
    void SetMemoryIndexer(UniquePtr<MemoryIndexer> &&memory_indexer) { memory_indexer_ = std::move(memory_indexer); }
    static SharedPtr<SegmentIndexEntry> CreateFakeEntry(const String &index_dir);

private:
    explicit SegmentIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Vector<BufferObj *> vector_buffer);
    // Load from disk. Is called by SegmentIndexEntry::Deserialize.
    static UniquePtr<SegmentIndexEntry>
    LoadIndexEntry(TableIndexEntry *table_index_entry, u32 segment_id, BufferManager *buffer_manager, CreateIndexParam *create_index_param);

    ChunkID GetNextChunkID() { return next_chunk_id_++; }

private:
    BufferManager *buffer_manager_{};
    TableIndexEntry *table_index_entry_;
    SharedPtr<String> index_dir_{};
    const SegmentID segment_id_{};

    Vector<BufferObj *> vector_buffer_{}; // size: 1 + GetIndexPartNum().

    std::shared_mutex rw_locker_{};

    TxnTimeStamp min_ts_{0}; // Indicate the commit_ts which create this SegmentIndexEntry
    TxnTimeStamp max_ts_{0}; // Indicate the max commit_ts which update data inside this SegmentIndexEntry
    TxnTimeStamp checkpoint_ts_{0};
    ChunkID next_chunk_id_{0};

    Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries_{};
    SharedPtr<ChunkIndexEntry> memory_hnsw_indexer_{};
    SharedPtr<MemoryIndexer> memory_indexer_{};
    SharedPtr<SecondaryIndexInMem> memory_secondary_index_{};
    SharedPtr<EMVBIndexInMem> memory_emvb_index_{};

    u64 ft_column_len_sum_{}; // increase only
    u32 ft_column_len_cnt_{}; // increase only
};

} // namespace infinity
