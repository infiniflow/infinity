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

#include <cstdio>

export module memory_indexer;
import stl;

import index_defines;
import posting_writer;
import column_vector;
import column_inverter;
import third_party;
import internal_types;
import ring;
import skiplist;
import internal_types;
import map_with_lock;
import vector_with_lock;
import buf_writer;
import posting_list_format;

namespace infinity {

export class MemoryIndexer {
public:
    struct KeyComp {
        bool operator()(const String &lhs, const String &rhs) const;
    };

    using PostingPtr = SharedPtr<PostingWriter>;
    // using PostingTableStore = SkipList<String, PostingPtr, KeyComp>;
    using PostingTableStore = MapWithLock<String, PostingPtr>;

    struct PostingTable {
        PostingTable();
        PostingTableStore store_;
    };

    MemoryIndexer(const String &index_dir, const String &base_name, RowID base_row_id, optionflag_t flag, const String &analyzer);

    ~MemoryIndexer();

    // Insert is non-blocking. Caller must ensure there's no RowID gap between each call.
    void Insert(SharedPtr<ColumnVector> column_vector, u32 row_offset, u32 row_count, bool offline = false);

    // InsertGap insert some empty documents. This is for abnormal case.
    void InsertGap(u32 row_count);

    // Commit is non-blocking and thread-safe. There shall be a background thread which call this method regularly.
    void Commit(bool offline = false);

    // CommitSync is for online case. It sort a batch of inverters, then generate posting for a batch of inverters.
    // Returns the batch size of generated posting.
    SizeT CommitSync(SizeT wait_if_empty_ms = 0);

    // Dump is blocking and shall be called only once after inserting all documents.
    // WARN: Don't reuse MemoryIndexer after calling Dump!
    void Dump(bool offline = false, bool spill = false);

    // A MemoryIndexer is allow to load iff it's empty or spilled.
    void Load();

    SizeT GetInflightTasks() {
        std::unique_lock<std::mutex> lock(mutex_);
        return inflight_tasks_;
    }

    String GetBaseName() const { return base_name_; }

    RowID GetBaseRowId() const { return base_row_id_; }

    u32 GetDocCount() const { return doc_count_; }

    u32 GetColumnLengthSum() const { return column_length_sum_.load(); }

    u32 GetColumnLength(u32 doc_id) { return column_lengths_.Get(doc_id); }

    SharedPtr<PostingTable> GetPostingTable() { return posting_table_; }

    SharedPtr<PostingWriter> GetOrAddPosting(const String &term);

    void Reset();

private:
    void WaitInflightTasks() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return inflight_tasks_ == 0; });
    }

    // CommitOffline is for offline case. It spill a batch of ColumnInverter. Returns the size of the batch.
    SizeT CommitOffline(SizeT wait_if_empty_ms = 0);

    void OfflineDump();

    void FinalSpillFile();

    void PrepareSpillFile();

    u32 ReadU32LE(const u8 *ptr) { return *(u32 *)ptr; }

    u64 ReadU64LE(const u8 *ptr) { return *(u64 *)ptr; }

private:
    String index_dir_;
    String base_name_;
    RowID base_row_id_{INVALID_ROWID};
    optionflag_t flag_;
    PostingFormat posting_format_;
    String analyzer_;
    ThreadPool &inverting_thread_pool_;
    ThreadPool &commiting_thread_pool_;
    u32 doc_count_{0};
    SharedPtr<PostingTable> posting_table_;
    PostingPtr prepared_posting_{nullptr};
    Ring<SharedPtr<ColumnInverter>> ring_inverted_;
    Ring<SharedPtr<ColumnInverter>> ring_sorted_;
    u64 seq_inserted_{0};
    u64 inflight_tasks_{0};
    std::condition_variable cv_;
    std::mutex mutex_;
    std::mutex mutex_commit_;

    u32 num_runs_{0};                  // For offline index building
    FILE *spill_file_handle_{nullptr}; // Temp file for offline external merge sort
    String spill_full_path_;           // Path of spill file
    u64 tuple_count_{0};               // Number of tuples for external merge sort

    bool is_spilled_{false};

    // for column length info
    VectorWithLock<u32> column_lengths_;
    Atomic<u32> column_length_sum_{0};

    // spill file write buf
    UniquePtr<char_t[]> spill_buffer_{};
    SizeT spill_buffer_size_{0};
    UniquePtr<BufWriter> buf_writer_;
};
} // namespace infinity
