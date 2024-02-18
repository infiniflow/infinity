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

export module memory_indexer;
import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import index_config;
import index_segment_reader;
import posting_writer;
import data_block;

import column_vector;
import analyzer;
import sequential_column_inverter;
import parallel_column_inverter;
import task_executor;
import memory_posting;
import third_party;
import internal_types;
import commit_task;

namespace vespalib::alloc {
class MemoryPoolAllocator;
}

namespace infinity {
class Indexer;
class ColumnIndexer;
export class MemoryIndexer {
public:
    using TermKey = String;
    // using PostingPtr = MemoryPosting<false> *;
    using PostingPtr = SharedPtr<PostingWriter>;
    using RTPostingPtr = SharedPtr<MemoryPosting<true>>;
    using PostingTable = Btree<TermKey, PostingPtr>;
    using RTPostingTable = Btree<TermKey, RTPostingPtr>;

    struct KeyComp {
        bool operator()(const String &lhs, const String &rhs) const;
        // bool operator()(const TermKey &lhs, const TermKey &rhs) const;
    };

    enum IndexMode {
        REAL_TIME,
        NEAR_REAL_TIME,
        OFFLINE,
    };

    MemoryIndexer(Indexer *indexer,
                  ColumnIndexer *column_indexer,
                  u64 column_id,
                  const InvertedIndexConfig &index_config,
                  SharedPtr<MemoryPool> byte_slice_pool,
                  SharedPtr<RecyclePool> buffer_pool);

    ~MemoryIndexer();

    void SetIndexMode(IndexMode index_mode);

    bool IsRealTime() { return index_mode_ == REAL_TIME; }
    // realtime insert
    void Insert(RowID row_id, String &data);

    void Insert(SharedPtr<ColumnVector> column_vector, RowID start_row_id);

    void PreCommit();

    void Commit();

    void TryDump();

    Analyzer *GetAnalyzer() { return analyzer_.get(); }

    bool IsJiebaSpecialize() { return jieba_specialize_; }

    MemoryPool *GetPool() { return byte_slice_pool_.get(); }

    PostingTable *GetPostingTable() { return posting_store_.get(); }

    RTPostingTable *GetRTPostingTable() { return rt_posting_store_.get(); }

    PostingPtr GetOrAddPosting(const TermKey &term);

    RTPostingPtr GetOrAddRTPosting(const TermKey &term);

    void ReclaimMemory();

    void Reset();

    void DisableCommit() { disable_commit_ = true; }

private:
    void SetAnalyzer();

    void SwitchActiveInverter();

    void SwitchActiveParallelInverters();

private:
    friend class ColumnIndexer;

    Indexer *indexer_{nullptr};
    ColumnIndexer *column_indexer_{nullptr};
    IndexMode index_mode_{NEAR_REAL_TIME};
    u64 column_id_;
    InvertedIndexConfig index_config_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    SharedPtr<vespalib::alloc::MemoryPoolAllocator> memory_allocator_;
    GenerationHandler generation_handler_;
    UniquePtr<PostingTable> posting_store_;
    UniquePtr<RTPostingTable> rt_posting_store_;
    UniquePtr<Analyzer> analyzer_;
    bool jieba_specialize_{false};
    Vector<UniquePtr<SequentialColumnInverter>> free_inverters_;
    Deque<UniquePtr<SequentialColumnInverter>> inflight_inverters_;
    UniquePtr<SequentialColumnInverter> inverter_;

    UniquePtr<ParallelColumnInverters> parallel_inverter_;
    Vector<UniquePtr<ParallelColumnInverters>> free_parallel_inverters_;
    Deque<UniquePtr<ParallelColumnInverters>> inflight_parallel_inverters_;

    UniquePtr<CommitTask> inflight_commit_task_;

    u32 num_inverters_;
    u32 max_inverters_;
    UniquePtr<SequencedTaskExecutor> invert_executor_;
    UniquePtr<SequencedTaskExecutor> commit_executor_;

    bool disable_commit_{false};
    std::condition_variable cv_;
    std::mutex mutex_;
};
} // namespace infinity
