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

#include "common/utility/sparse_gram.h"

export module infinity_core:column_index_reader;

import :segment_posting;
import :index_segment_reader;
import :posting_iterator;
import :index_defines;
import :logger;
import :status;
import :default_values;

import third_party;

import internal_types;

namespace infinity {
export class TermDocIterator;
class NewTxn;
class MemoryIndexer;
class TableIndexMeta;
class BufferObj;
struct SegmentIndexFtInfo;

struct ColumnReaderChunkInfo {
    BufferObj *index_buffer_ = nullptr;
    RowID base_rowid_{};
    size_t row_cnt_{};
    size_t term_cnt_{};
    SegmentID segment_id_ = 0;
    ChunkID chunk_id_ = 0;
};

export class ColumnIndexReader {
public:
    ~ColumnIndexReader();

    Status Open(optionflag_t flag, TableIndexMeta &table_index_meta);

    std::unique_ptr<PostingIterator> Lookup(const std::string &term, bool fetch_position = true);

    optionflag_t GetOptionFlag() const { return flag_; }

    void InvalidateSegment(SegmentID segment_id);

    void InvalidateChunk(SegmentID segment_id, ChunkID chunk_id);

    inline std::string GetColumnName() const { return column_name_; }

    inline std::string GetIndexName() const { return index_name_; }

    inline std::string GetAnalyzer() const { return analyzer_; }

private:
    std::mutex mutex_;

    optionflag_t flag_;
    std::vector<std::shared_ptr<IndexSegmentReader>> segment_readers_;

public:
    std::string column_name_;
    std::string index_name_;
    std::string analyzer_;
    // for loading column length files
    std::string index_dir_;
    std::vector<std::shared_ptr<MemoryIndexer>> memory_indexers_;
    std::vector<ColumnReaderChunkInfo> chunk_index_meta_infos_;
};

namespace detail {
template <class T>
struct Hash {
    inline size_t operator()(const T &val) const { return val; }
};
} // namespace detail

export struct IndexReader {
    // The index an unqualified full-text predicate on a column goes to.
    //
    // A column may carry more than one full-text index -- typically a regular
    // analyzer for searching and a sparse gram analyzer for `regex()` filters.
    // A predicate that names no index is written in the tokens of the regular
    // analyzer, so that is the index it uses; a sparse gram index only answers
    // for a column that has no other. The analyzer that parses the query text
    // follows the same rule, so it and the reader never disagree.
    std::string GetDefaultIndexName(const std::string &column_name) const {
        const auto column_index_map = column_index_readers_.find(column_name);
        if (column_index_map == column_index_readers_.end() || column_index_map->second.empty()) {
            return {};
        }
        std::string first_index_name;
        for (const auto &[index_name, reader] : column_index_map->second) {
            SparseGramParams params;
            if (!ParseSparseGramAnalyzerName(reader->GetAnalyzer(), params)) {
                return index_name;
            }
            if (first_index_name.empty()) {
                first_index_name = index_name;
            }
        }
        return first_index_name;
    }

    // Get a index reader on a column based on hints.
    // If no such column exists, return nullptr.
    // If column exists, but no index with a hint name is found, return nullptr.
    // A hint without a name resolves to the default index of the column.
    ColumnIndexReader *GetColumnIndexReader(const std::string &column_name, const std::string &index_name = "") const {
        const auto &column_index_map = column_index_readers_.find(column_name);
        // if no fulltext index exists, or the map is empty.
        if (column_index_map == column_index_readers_.end() || column_index_map->second.empty()) {
            return nullptr;
        }

        auto &indices_map = column_index_map->second;
        if (indices_map.empty()) {
            return nullptr;
        }
        if (index_name.length() == 0) {
            auto it = indices_map.find(GetDefaultIndexName(column_name));
            if (it != indices_map.end()) {
                return it->second.get();
            }
            return nullptr;
        } else {
            auto it = indices_map.find(index_name);
            if (it != indices_map.end()) {
                return it->second.get();
            }
            return nullptr;
        }
    }

    // column_name -> [index_name -> analyzer]
    std::map<std::string, std::map<std::string, std::string>> GetColumn2Analyzer() const {
        std::map<std::string, std::map<std::string, std::string>> rst;
        for (const auto &column_index_map : column_index_readers_) {
            const std::string &column_name = column_index_map.first;
            std::map<std::string, std::string> index2Analyzer;
            for (const auto &column_index_reader : column_index_map.second) {
                const std::string &index_name = column_index_reader.first;
                const std::string &analyzer = column_index_reader.second->GetAnalyzer();
                index2Analyzer[index_name] = analyzer;
            }
            rst.emplace(column_name, index2Analyzer);
        }
        return rst;
    }

    // column_name -> [index_name -> column_index_reader]
    std::map<std::string, std::map<std::string, std::shared_ptr<ColumnIndexReader>>> column_index_readers_;
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
    std::map<std::string, std::map<std::string, std::shared_ptr<ColumnIndexReader>>> cache_column_readers_;
};

} // namespace infinity
