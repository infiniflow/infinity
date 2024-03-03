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
import column_inverter;
import third_party;
import internal_types;
import ring;

namespace vespalib::alloc {
class MemoryPoolAllocator;
}

namespace infinity {
class ColumnIndexer;
export class MemoryIndexer {
public:
    using TermKey = String;
    using PostingPtr = SharedPtr<PostingWriter>;
    using PostingTable = HashMap<TermKey, PostingPtr>;

    struct KeyComp {
        bool operator()(const String &lhs, const String &rhs) const;
    };

    enum IndexMode {
        NEAR_REAL_TIME,
        OFFLINE,
    };

    MemoryIndexer(u64 column_id,
                  const InvertedIndexConfig &index_config,
                  SharedPtr<MemoryPool> byte_slice_pool,
                  SharedPtr<RecyclePool> buffer_pool,
                  ThreadPool &thread_pool);

    ~MemoryIndexer();

    void Insert(const ColumnVector &column_vector, u32 row_offset, u32 row_count, RowID row_id_begin);

    void Commit();

    void WaitInflightTasks() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return inflight_tasks_ == 0; });
    }

    void SetIndexMode(IndexMode index_mode);

    Analyzer *GetAnalyzer() { return analyzer_.get(); }

    bool IsJiebaSpecialize() { return jieba_specialize_; }

    MemoryPool *GetPool() { return byte_slice_pool_.get(); }

    PostingTable *GetPostingTable() { return posting_store_.get(); }

    PostingPtr GetOrAddPosting(const TermKey &term);

    void Reset();

private:
    void SetAnalyzer();

private:
    friend class ColumnIndexer;

    IndexMode index_mode_{NEAR_REAL_TIME};
    u64 column_id_;
    InvertedIndexConfig index_config_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    UniquePtr<PostingTable> posting_store_;
    UniquePtr<Analyzer> analyzer_;
    bool jieba_specialize_{false};

    ThreadPool &thread_pool_;
    Ring<SharedPtr<ColumnInverter>> ring_inverted_;
    Ring<SharedPtr<ColumnInverter>> ring_sorted_;
    u64 seq_inserted_{0};
    u64 inflight_tasks_{0};

    std::condition_variable cv_;
    std::mutex mutex_;
};
} // namespace infinity
