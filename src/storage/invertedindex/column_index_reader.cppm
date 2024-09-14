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

import third_party;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import memory_indexer;
import internal_types;
import segment_index_entry;
import chunk_index_entry;

export module column_index_reader;

namespace infinity {
struct TableEntry;
class TermDocIterator;
class Txn;

export class ColumnIndexReader {
public:
    void Open(optionflag_t flag, String &&index_dir, Map<SegmentID, SharedPtr<SegmentIndexEntry>> &&index_by_segment, Txn *txn);

    UniquePtr<PostingIterator> Lookup(const String &term, bool fetch_position = true);

    Pair<u64, float> GetTotalDfAndAvgColumnLength();

    optionflag_t GetOptionFlag() const { return flag_; }
private:
    optionflag_t flag_;
    Vector<SharedPtr<IndexSegmentReader>> segment_readers_;
    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment_;
    u64 total_df_ = 0;
    float avg_column_length_ = 0.0f;

public:
    // for loading column length files
    String index_dir_;
    Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries_;
    SharedPtr<MemoryIndexer> memory_indexer_{nullptr};
};

namespace detail {
template <class T>
struct Hash {
    inline SizeT operator()(const T &val) const { return val; }
};
} // namespace detail

export struct IndexReader {
    ColumnIndexReader *GetColumnIndexReader(u64 column_id) const { return (*column_index_readers_)[column_id].get(); }
    const Map<String, String> &GetColumn2Analyzer() const { return *column2analyzer_; }

    SharedPtr<FlatHashMap<u64, SharedPtr<ColumnIndexReader>, detail::Hash<u64>>> column_index_readers_;
    SharedPtr<Map<String, String>> column2analyzer_;
};

export class TableIndexReaderCache {
public:
    inline explicit TableIndexReaderCache(TableEntry *table_entry_ptr) : table_entry_ptr_(table_entry_ptr) {}

    void UpdateKnownUpdateTs(TxnTimeStamp ts, std::shared_mutex &segment_update_ts_mutex, TxnTimeStamp &segment_update_ts);

    IndexReader GetIndexReader(Txn *txn);

    // User shall call this function only once when all transactions using `GetIndexReader()` have finished.
    void Invalidate();

private:
    std::mutex mutex_;
    TableEntry *table_entry_ptr_ = nullptr;
    TxnTimeStamp first_known_update_ts_ = 0;
    TxnTimeStamp last_known_update_ts_ = 0;
    TxnTimeStamp cache_ts_ = 0;
    FlatHashMap<u64, TxnTimeStamp, detail::Hash<u64>> cache_column_ts_;
    SharedPtr<FlatHashMap<u64, SharedPtr<ColumnIndexReader>, detail::Hash<u64>>> cache_column_readers_;
    SharedPtr<Map<String, String>> column2analyzer_;
};

} // namespace infinity