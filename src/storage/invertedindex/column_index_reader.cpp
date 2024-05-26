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
#include <vector>

module column_index_reader;

import stl;

import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import disk_index_segment_reader;
import inmem_index_segment_reader;
import memory_indexer;
import dict_reader;
import posting_list_format;
import internal_types;
import segment_index_entry;
import infinity_exception;
import table_entry;
import create_index_info;
import index_base;
import index_full_text;
import third_party;
import blockmax_term_doc_iterator;
import default_values;

namespace infinity {
void ColumnIndexReader::Open(optionflag_t flag, String &&index_dir, Map<SegmentID, SharedPtr<SegmentIndexEntry>> &&index_by_segment) {
    flag_ = flag;
    index_dir_ = std::move(index_dir);
    index_by_segment_ = std::move(index_by_segment);
    // need to ensure that segment_id is in ascending order
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        auto [chunk_index_entries, memory_indexer] = segment_index_entry->GetFullTextIndexSnapshot();
        // segment_readers
        for (u32 i = 0; i < chunk_index_entries.size(); ++i) {
            SharedPtr<DiskIndexSegmentReader> segment_reader =
                MakeShared<DiskIndexSegmentReader>(index_dir_, chunk_index_entries[i]->base_name_, chunk_index_entries[i]->base_rowid_, flag);
            segment_readers_.push_back(std::move(segment_reader));
        }
        chunk_index_entries_.insert(chunk_index_entries_.end(),
                                    std::move_iterator(chunk_index_entries.begin()),
                                    std::move_iterator(chunk_index_entries.end()));
        if (memory_indexer.get() != nullptr && memory_indexer->GetDocCount() != 0) {
            // segment_reader
            SharedPtr<InMemIndexSegmentReader> segment_reader = MakeShared<InMemIndexSegmentReader>(memory_indexer.get());
            segment_readers_.push_back(std::move(segment_reader));
            // for loading column length file
            assert(memory_indexer_.get() == nullptr);
            memory_indexer_ = memory_indexer;
        }
    }
}

UniquePtr<PostingIterator> ColumnIndexReader::Lookup(const String &term, bool fetch_position) {
    SharedPtr<Vector<SegmentPosting>> seg_postings = MakeShared<Vector<SegmentPosting>>();
    for (u32 i = 0; i < segment_readers_.size(); ++i) {
        SegmentPosting seg_posting;
        auto ret = segment_readers_[i]->GetSegmentPosting(term, seg_posting, fetch_position);
        if (ret) {
            seg_postings->push_back(seg_posting);
        }
    }
    if (seg_postings->empty()) {
        return nullptr;
    }
    auto iter = MakeUnique<PostingIterator>(flag_);
    u32 state_pool_size = 0; // TODO
    iter->Init(std::move(seg_postings), state_pool_size);
    return iter;
}

UniquePtr<BlockMaxTermDocIterator> ColumnIndexReader::LookupBlockMax(const String &term, float weight, bool fetch_position) {
    SharedPtr<Vector<SegmentPosting>> seg_postings = MakeShared<Vector<SegmentPosting>>();
    for (u32 i = 0; i < segment_readers_.size(); ++i) {
        SegmentPosting seg_posting;
        auto ret = segment_readers_[i]->GetSegmentPosting(term, seg_posting, fetch_position);
        if (ret) {
            seg_postings->push_back(seg_posting);
        }
    }
    if (seg_postings->empty()) {
        return nullptr;
    }
    auto result = MakeUnique<BlockMaxTermDocIterator>(flag_);
    result->MultiplyWeight(weight);
    u32 state_pool_size = 0; // TODO
    result->InitPostingIterator(std::move(seg_postings), state_pool_size);
    return result;
}

float ColumnIndexReader::GetAvgColumnLength() const {
    u64 column_len_sum = 0;
    u32 column_len_cnt = 0;
    for (const auto &[segment_id, segment_index_entry] : index_by_segment_) {
        auto [sum, cnt] = segment_index_entry->GetFulltextColumnLenInfo();
        column_len_sum += sum;
        column_len_cnt += cnt;
    }
    if (column_len_cnt == 0) {
        UnrecoverableError("column_len_cnt is 0");
    }
    return static_cast<float>(column_len_sum) / column_len_cnt;
}

void TableIndexReaderCache::UpdateKnownUpdateTs(TxnTimeStamp ts, std::shared_mutex &segment_update_ts_mutex, TxnTimeStamp &segment_update_ts) {
    std::scoped_lock lock1(mutex_);
    std::unique_lock lock2(segment_update_ts_mutex);
    if (ts < segment_update_ts) {
        // Optimize txn begin ts may be less than Insert txn commit ts
        return;
    }
    segment_update_ts = ts;
    first_known_update_ts_ = std::min(first_known_update_ts_, ts);
    last_known_update_ts_ = std::max(last_known_update_ts_, ts);
}

IndexReader TableIndexReaderCache::GetIndexReader(Txn *txn, TableEntry *self_table_entry_ptr) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    TransactionID txn_id = txn->TxnID();
    IndexReader result;
    std::scoped_lock lock(mutex_);
    assert(cache_ts_ <= first_known_update_ts_);
    assert(first_known_update_ts_ == MAX_TIMESTAMP || first_known_update_ts_ <= last_known_update_ts_);
    if (first_known_update_ts_ != 0 && begin_ts >= cache_ts_ && begin_ts < first_known_update_ts_) [[likely]] {
        // no need to build, use cache
        result.column_index_readers_ = cache_column_readers_;
        result.column2analyzer_ = column2analyzer_;
    } else {
        FlatHashMap<u64, TxnTimeStamp, detail::Hash<u64>> cache_column_ts;
        result.column_index_readers_ = MakeShared<FlatHashMap<u64, SharedPtr<ColumnIndexReader>, detail::Hash<u64>>>();
        result.column2analyzer_ = MakeShared<Map<String, String>>();
        for (auto map_guard = self_table_entry_ptr->IndexMetaMap(); auto &[index_name, table_index_meta] : *map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                // Table index entry isn't found
                RecoverableError(status);
            }
            // check index type
            const IndexBase *index_base = table_index_entry->index_base();
            if (auto index_type = index_base->index_type_; index_type != IndexType::kFullText) {
                // non-fulltext index
                continue;
            }
            String column_name = index_base->column_name();
            auto column_id = self_table_entry_ptr->GetColumnIdByName(column_name);
            assert(table_index_entry->GetFulltexSegmentUpdateTs() <= last_known_update_ts_);
            if (auto &target_ts = cache_column_ts[column_id]; target_ts < begin_ts) {
                // need update result
                target_ts = begin_ts;
                const IndexFullText *index_full_text = reinterpret_cast<const IndexFullText *>(index_base);
                // update column2analyzer_
                (*result.column2analyzer_)[column_name] = index_full_text->analyzer_;
                if (auto it = cache_column_ts_.find(column_id); it != cache_column_ts_.end() and it->second == begin_ts) {
                    // reuse cache
                    (*result.column_index_readers_)[column_id] = cache_column_readers_->at(column_id);
                } else {
                    // new column_index_reader
                    auto column_index_reader = MakeShared<ColumnIndexReader>();
                    optionflag_t flag = index_full_text->flag_;
                    String index_dir = *(table_index_entry->index_dir());
                    Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment =
                        table_index_entry->GetIndexBySegmentSnapshot(self_table_entry_ptr, txn);
                    column_index_reader->Open(flag, std::move(index_dir), std::move(index_by_segment));
                    (*result.column_index_readers_)[column_id] = std::move(column_index_reader);
                }
            }
        }
        if (begin_ts >= last_known_update_ts_) {
            // need to update cache
            cache_ts_ = last_known_update_ts_;
            first_known_update_ts_ = MAX_TIMESTAMP;
            last_known_update_ts_ = 0;
            cache_column_ts_ = std::move(cache_column_ts);
            cache_column_readers_ = result.column_index_readers_;
            column2analyzer_ = result.column2analyzer_;
        }
    }
    return result;
}

} // namespace infinity
