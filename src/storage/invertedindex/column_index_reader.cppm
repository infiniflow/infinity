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

export module infinity_core:column_index_reader;

import :segment_posting;
import :index_segment_reader;
import :posting_iterator;
import :index_defines;
import :logger;
import :status;
import :default_values;
import :file_worker;

import third_party;

import internal_types;

namespace infinity {
export class TermDocIterator;
class NewTxn;
class MemoryIndexer;
class TableIndexMeta;
struct SegmentIndexFtInfo;

struct ColumnReaderChunkInfo {
    FileWorker *index_buffer_{};
    RowID base_rowid_{};
    size_t row_cnt_{};
    size_t term_cnt_{};
    SegmentID segment_id_{};
    ChunkID chunk_id_{};
};

export class ColumnIndexReader {
public:
    ~ColumnIndexReader();

    Status Open(optionflag_t flag, TableIndexMeta &table_index_meta);

    std::unique_ptr<PostingIterator> Lookup(const std::string &term, bool fetch_position = true);

    optionflag_t GetOptionFlag() const { return flag_; }

    void InvalidateSegment(SegmentID segment_id);

    void InvalidateChunk(SegmentID segment_id, ChunkID chunk_id);

    inline std::string GetAnalyzer() const { return analyzer_; }

    inline std::string GetColumnName() const { return column_name_; }

private:
    std::mutex mutex_;

    optionflag_t flag_;
    std::vector<std::shared_ptr<IndexSegmentReader>> segment_readers_;

public:
    std::string column_name_;
    std::string analyzer_;
    // for loading column length files
    std::string index_dir_;
    std::shared_ptr<MemoryIndexer> memory_indexer_{nullptr};

    std::vector<ColumnReaderChunkInfo> chunk_index_meta_infos_;
};

namespace detail {
template <class T>
struct Hash {
    inline size_t operator()(const T &val) const { return val; }
};
} // namespace detail

export struct IndexReader {
    // Get a index reader on a column based on hints.
    // If no such column exists, return nullptr.
    // If column exists, but no index with a hint name is found, return a random one.
    ColumnIndexReader *GetColumnIndexReader(u64 column_id, const std::vector<std::string> &hints) const {
        const auto &column_index_map = column_index_readers_->find(column_id);
        // if no fulltext index exists, or the map is empty.
        if (column_index_map == column_index_readers_->end() || column_index_map->second->size() == 0) {
            return nullptr;
        }

        auto indices_map = column_index_map->second;
        for (size_t i = 0; i < hints.size(); i++) {
            if (auto it = indices_map->find(hints[i]); it != indices_map->end()) {
                return indices_map->at(hints[i]).get();
            }
        }
        return indices_map->begin()->second.get();
    }

    // return map: column_name -> analyzer_name based on hints.
    std::map<std::string, std::string> GetColumn2Analyzer(const std::vector<std::string> &hints) const {
        std::map<std::string, std::string> rst;
        for (const auto &id_index_map : *column_index_readers_) {
            ColumnIndexReader *column_index_reader = GetColumnIndexReader(id_index_map.first, hints);
            if (column_index_reader != nullptr) {
                rst[column_index_reader->GetColumnName()] = column_index_reader->GetAnalyzer();
            }
        }
        return rst;
    }

    // column_id -> [index_name -> column_index_reader]
    std::shared_ptr<FlatHashMap<u64, std::shared_ptr<std::map<std::string, std::shared_ptr<ColumnIndexReader>>>, detail::Hash<u64>>>
        column_index_readers_;
};

export class TableIndexReaderCache {
public:
    TableIndexReaderCache(const std::string &db_id_str, const std::string &table_id_str, const std::string &table_name)
        : db_id_str_(db_id_str), table_id_str_(table_id_str), table_name_(table_name) {}

    std::shared_ptr<IndexReader> GetIndexReader(NewTxn *txn);

    // User shall call this function only once when all transactions using `GetIndexReader()` have finished.
    void Invalidate();

private:
    std::mutex mutex_;
    std::string db_id_str_;
    std::string table_id_str_;
    std::string table_name_;

    TxnTimeStamp cache_ts_ = UNCOMMIT_TS;
    std::shared_ptr<FlatHashMap<u64, std::shared_ptr<std::map<std::string, std::shared_ptr<ColumnIndexReader>>>, detail::Hash<u64>>>
        cache_column_readers_;
};

} // namespace infinity
