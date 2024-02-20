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
import memory_pool;
import segment_posting;
import posting_iterator;
import index_defines;
import index_config;
import index_segment_reader;
import inmem_index_segment_reader;
import posting_writer;
import block_entry;
import buffer_manager;
import column_indexer;
import third_party;
import segment;
import internal_types;
export module indexer;

namespace infinity {

struct TableEntry;

namespace detail {
template <class T>
struct Hash {
    inline SizeT operator()(const T &val) const { return val; }
};
} // namespace detail

export class Indexer {
public:
    Indexer();

    ~Indexer();

    void Open(const InvertedIndexConfig &index_config, const String &directory);

    // Exactly one thread can call this method
    void BatchInsert(const BlockEntry *block_entry, u32 row_offset, u32 row_count, BufferManager *buffer_mgr);

    void Commit();

    void TryDump();

    void Dump();

    SharedPtr<InMemIndexSegmentReader> CreateInMemSegmentReader(u64 column_id);

    String GetDirectory() { return directory_; }

    void GetSegments(Vector<Segment> &segments);

    segmentid_t GetNextSegmentID() { return id_generator_->GetNextSegmentID(); }

    ColumnIndexer *GetColumnIndexer(u64 column_id) { return column_indexers_[column_id].get(); }

    std::shared_mutex &GetMutex() { return flush_mutex_; }

    const Vector<u64> &GetColumnIDs() const { return column_ids_; }

    const InvertedIndexConfig &GetIndexConfig() const { return index_config_; }

private:
    void AddSegment();

    void UpdateSegment(RowID row_id, u64 inc_count = 1);

    InvertedIndexConfig index_config_;
    String directory_;
    Vector<u64> column_ids_;
    SharedPtr<MemoryPool> byte_slice_pool_;
    SharedPtr<RecyclePool> buffer_pool_;
    FlatHashMap<u64, UniquePtr<ColumnIndexer>, detail::Hash<u64>> column_indexers_;
    SharedPtr<IDGenerator> id_generator_;
    Vector<Segment> segments_;
    Atomic<Segment *> active_segment_;
    Atomic<u32> dump_ref_count_;
    mutable std::shared_mutex flush_mutex_;
};
} // namespace infinity
