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

export module column_index_reader;

import stl;
import third_party;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
// import memory_indexer;
import internal_types;
import segment_index_entry;
import chunk_index_entry;
import logger;

namespace infinity {
struct TableEntry;
class TermDocIterator;
class Txn;
class MemoryIndexer;

export class ColumnIndexReader {
public:
    void Open(optionflag_t flag, String &&index_dir, Map<SegmentID, SharedPtr<SegmentIndexEntry>> &&index_by_segment, Txn *txn);

    UniquePtr<PostingIterator> Lookup(const String &term, bool fetch_position = true);

    Pair<u64, float> GetTotalDfAndAvgColumnLength();

    optionflag_t GetOptionFlag() const { return flag_; }

    void InvalidateSegment(SegmentID segment_id);

    void InvalidateChunk(SegmentID segment_id, ChunkID chunk_id);

    inline String GetAnalyzer() const { return analyzer_; }

    inline String GetColumnName() const { return column_name_; }

private:
    std::mutex mutex_;

    optionflag_t flag_;
    Vector<SharedPtr<IndexSegmentReader>> segment_readers_;
    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment_;
    u64 total_df_ = 0;
    float avg_column_length_ = 0.0f;

public:
    String column_name_;
    String analyzer_;
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
    // Get a index reader on a column based on hints.
    // If no such column exists, return nullptr.
    // If column exists, but no index with a hint name is found, return a random one.
    ColumnIndexReader *GetColumnIndexReader(u64 column_id, const Vector<String> &hints) const {
        const auto &column_index_map = column_index_readers_->find(column_id);
        // if no fulltext index exists, or the map is empty.
        if (column_index_map == column_index_readers_->end() || column_index_map->second->size() == 0) {
            return nullptr;
        }

        auto indices_map = column_index_map->second;
        for (SizeT i = 0; i < hints.size(); i++) {
            if (auto it = indices_map->find(hints[i]); it != indices_map->end()) {
                return indices_map->at(hints[i]).get();
            }
        }
        return indices_map->begin()->second.get();
    }

    // return map: column_name -> analyzer_name based on hints.
    Map<String, String> GetColumn2Analyzer(const Vector<String> &hints) const {
        Map<String, String> rst;
        for (const auto &id_index_map : *column_index_readers_) {
            ColumnIndexReader *column_index_reader = GetColumnIndexReader(id_index_map.first, hints);
            if (column_index_reader != nullptr) {
                rst[column_index_reader->GetColumnName()] = column_index_reader->GetAnalyzer();
            }
        }
        return rst;
    }

    // column_id -> [index_name -> column_index_reader]
    SharedPtr<FlatHashMap<u64, SharedPtr<Map<String, SharedPtr<ColumnIndexReader>>>, detail::Hash<u64>>> column_index_readers_;
};

export class TableIndexReaderCache {
public:
    inline explicit TableIndexReaderCache(TableEntry *table_entry_ptr) : table_entry_ptr_(table_entry_ptr) {}

    void UpdateKnownUpdateTs(TxnTimeStamp ts, std::shared_mutex &segment_update_ts_mutex, TxnTimeStamp &segment_update_ts);

    SharedPtr<IndexReader> GetIndexReader(Txn *txn);

    // User shall call this function only once when all transactions using `GetIndexReader()` have finished.
    void Invalidate();

    void InvalidateColumn(u64 column_id, const String &column_name);

    void InvalidateSegmentColumn(u64 column_id, SegmentID segment_id);

    void InvalidateChunkColumn(u64 column_id, SegmentID segment_id, ChunkID chunk_id);

private:
    std::mutex mutex_;
    TableEntry *table_entry_ptr_ = nullptr;
    TxnTimeStamp first_known_update_ts_ = 0;
    TxnTimeStamp last_known_update_ts_ = 0;
    TxnTimeStamp cache_ts_ = 0;
    FlatHashMap<u64, TxnTimeStamp, detail::Hash<u64>> cache_column_ts_;
    SharedPtr<FlatHashMap<u64, SharedPtr<Map<String, SharedPtr<ColumnIndexReader>>>, detail::Hash<u64>>> cache_column_readers_;
};

} // namespace infinity
