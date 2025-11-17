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

export module infinity_core:memory_indexer;

import :index_defines;
import :posting_writer;
import :column_vector;
import :column_inverter;
import :ring;
import :skiplist;
import :map_with_lock;
import :vector_with_lock;
import :buf_writer;
import :posting_list_format;
import :external_sort_merger;
import :persistence_manager;
import :base_memindex;
import :memindex_tracer;
import :mem_usage_change;
import :chunk_index_meta;
import :rcu_multimap;

import std.compat;
import third_party;

import internal_types;

namespace infinity {

struct AppendMemIndexBatch;
class AppendMemIndexTask;

export class MemoryIndexer final : public BaseMemIndex {
public:
    void ApplyMemUseChange(MemUsageChange mem_change);

    struct KeyComp {
        bool operator()(const std::string &lhs, const std::string &rhs) const;
    };

    using PostingPtr = std::shared_ptr<PostingWriter>;
    // using PostingTableStore = SkipList<std::string, PostingPtr, KeyComp>;
    // using PostingTableStore = MapWithLock<std::string, PostingPtr>;
    using PostingTableStore = RcuMap<std::string, PostingPtr>;

    struct PostingTable {
        PostingTable();
        PostingTableStore store_;
    };

    MemoryIndexer(const std::string &index_dir, const std::string &base_name, RowID base_row_id, optionflag_t flag, const std::string &analyzer);

    ~MemoryIndexer();

    // Insert is non-blocking. Caller must ensure there's no RowID gap between each call.
    void Insert(std::shared_ptr<ColumnVector> column_vector, u32 row_offset, u32 row_count, bool offline = false);

    void AsyncInsertTop(AppendMemIndexTask *);

    void AsyncInsertBottom(const std::shared_ptr<ColumnVector> &column_vector,
                           u32 row_offset,
                           u32 row_count,
                           u64 seq_inserted,
                           u32 doc_count,
                           AppendMemIndexBatch *append_batch);

    std::unique_ptr<std::binary_semaphore> AsyncInsert(std::shared_ptr<ColumnVector> column_vector, u32 row_offset, u32 row_count);

    // InsertGap insert some empty documents. This is for abnormal case.
    void InsertGap(u32 row_count);

    // Commit is non-blocking and thread-safe. There shall be a background thread which call this method regularly.
    void Commit(bool offline = false);

    // CommitSync is for online case. It sort a batch of inverters, then generate posting for a batch of inverters.
    // Returns the batch size of generated posting.
    size_t CommitSync(size_t wait_if_empty_ms = 0);

    // Dump is blocking and shall be called only once after inserting all documents.
    // WARN: Don't reuse MemoryIndexer after calling Dump!
    void Dump(bool offline = false, bool spill = false);

    // A MemoryIndexer is allow to load iff it's empty or spilled.
    void Load();

    size_t GetInflightTasks() {
        std::unique_lock<std::mutex> lock(mutex_);
        return inflight_tasks_;
    }

    void WaitInflightTasks() {
        while (GetInflightTasks() > 0) {
            CommitSync(100);
        }
    }

    std::string GetBaseName() const { return base_name_; }

    RowID GetBeginRowID() const override { return base_row_id_; }

    u32 GetDocCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return doc_count_;
    }

    std::pair<size_t, size_t> GetDocTermCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return {doc_count_, term_cnt_.load()};
    }

    u32 GetColumnLength(u32 doc_id) { return column_lengths_.Get(doc_id); }

    std::shared_ptr<PostingTable> GetPostingTable() { return posting_table_; }

    std::shared_ptr<PostingWriter> GetOrAddPosting(const std::string &term);

    void Reset();

    MemIndexTracerInfo GetInfo() const override;

    const ChunkIndexMetaInfo GetChunkIndexMetaInfo() const override;

    size_t MemUsed() const;

private:
    // call with write lock
    void IncreaseMemoryUsage(size_t mem);

    // call with write lock
    void DecreaseMemoryUsage(size_t mem);

    // CommitOffline is for offline case. It spill a batch of ColumnInverter. Returns the size of the batch.
    size_t CommitOffline(size_t wait_if_empty_ms = 0);

    void OfflineDump();

    void FinalSpillFile();

    void PrepareSpillFile();

    u32 ReadU32LE(const u8 *ptr) { return *(u32 *)ptr; }

    u64 ReadU64LE(const u8 *ptr) { return *(u64 *)ptr; }

    void TupleListToIndexFile(std::unique_ptr<SortMergerTermTuple<TermTuple, u32>> &merger);

private:
    std::string index_dir_;
    std::string base_name_;
    RowID base_row_id_{INVALID_ROWID};
    optionflag_t flag_;
    PostingFormat posting_format_;
    std::string analyzer_;
    ctpl::thread_pool &inverting_thread_pool_;
    ctpl::thread_pool &commiting_thread_pool_;
    u32 doc_count_{0};
    std::shared_ptr<PostingTable> posting_table_;
    PostingPtr prepared_posting_{nullptr};
    Ring<std::shared_ptr<ColumnInverter>> ring_sorted_;
    u64 seq_inserted_{0};
    u64 inflight_tasks_{0};
    std::condition_variable cv_;
    mutable std::mutex mutex_;
    std::mutex mutex_commit_;
    std::shared_mutex mutex_commit_sync_share_;

    u32 num_runs_{};              // For offline index building
    FILE *spill_file_handle_{};   // Temp file for offline external merge sort
    std::string spill_full_path_; // Path of spill file
    u64 tuple_count_{0};          // Number of tuples for external merge sort

    bool is_spilled_{};

    // for column length info
    VectorWithLock<u32> column_lengths_;
    std::atomic<u32> term_cnt_{};

    // spill file write buf
    std::unique_ptr<char[]> spill_buffer_;
    size_t spill_buffer_size_{};
    std::unique_ptr<BufWriter> buf_writer_;

    std::atomic<size_t> mem_used_{};
};
} // namespace infinity
