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
#include <memory>

module infinity_core:column_index_reader.impl;

import :column_index_reader;

import :stl;
import :segment_posting;
import :index_segment_reader;
import :posting_iterator;
import :index_defines;
import :disk_index_segment_reader;
import :inmem_index_segment_reader;
import :memory_indexer;
import :dict_reader;
import :posting_list_format;
import internal_types;
import :infinity_exception;
import create_index_info;
import :index_base;
import :index_full_text;
import :third_party;
import :term_doc_iterator;
import :default_values;
import :logger;
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :new_txn;
import :status;
import :mem_index;
import :kv_store;

import :new_catalog;
import :buffer_handle;

namespace infinity {

ColumnIndexReader::~ColumnIndexReader() = default;

Status ColumnIndexReader::Open(optionflag_t flag, TableIndexMeeta &table_index_meta) {
    flag_ = flag;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    {
        Status status;
        std::tie(segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
        if (!status.ok()) {
            return status;
        }
    }
    if (segment_ids_ptr->empty()) {
        KVInstance &kv_instance = table_index_meta.kv_instance();
        LOG_INFO(fmt::format("All kv_instance key and value: {}", kv_instance.ToString()));
    }
    u64 column_len_sum = 0;
    u32 column_len_cnt = 0;
    // need to ensure that segment_id is in ascending order
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta);
        SharedPtr<String> index_dir = segment_index_meta.GetSegmentIndexDir();
        SharedPtr<SegmentIndexFtInfo> ft_info_ptr;
        Status status = segment_index_meta.GetFtInfo(ft_info_ptr);
        if (!status.ok()) {
            return status;
        }
        RowID ft_info_next_rowid = RowID(segment_index_meta.segment_id(), ft_info_ptr->ft_column_len_cnt_);

        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        std::tie(chunk_ids_ptr, status) = segment_index_meta.GetChunkIDs1();
        if (!status.ok()) {
            return status;
        }

        for (ChunkID chunk_id : *chunk_ids_ptr) {
            ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            {
                status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
                if (!status.ok()) {
                    return status;
                }
            }
            SharedPtr<DiskIndexSegmentReader> segment_reader =
                MakeShared<DiskIndexSegmentReader>(segment_id, chunk_id, *index_dir, chunk_info_ptr->base_name_, chunk_info_ptr->base_row_id_, flag);
            segment_readers_.push_back(std::move(segment_reader));

            BufferObj *index_buffer = nullptr;
            status = chunk_index_meta.GetIndexBuffer(index_buffer);
            if (!status.ok()) {
                return status;
            }

            chunk_index_meta_infos_.emplace_back(
                ColumnReaderChunkInfo{index_buffer, chunk_info_ptr->base_row_id_, u32(chunk_info_ptr->row_cnt_), chunk_id, segment_id});
            if (chunk_info_ptr->base_row_id_ >= ft_info_next_rowid) {
                // KV ft_info doesn't cover this chunk due to shutdown before checkpoint.
                // NewCatalog::LoadFlushedChunkIndex1 isn't responsible to rectify it.
                // So we need to rectify it here just before query.
                // Refers to FullTextColumnLengthReader::SeekFile(RowID row_id)
                u64 chunk_column_len_sum = 0;
                BufferHandle chunk_buffer_handle = index_buffer->Load();
                auto column_lengths = (const u32 *)chunk_buffer_handle.GetData();
                for (SizeT i = 0; i < chunk_info_ptr->row_cnt_; i++) {
                    chunk_column_len_sum += column_lengths[i];
                }
                column_len_sum += chunk_column_len_sum;
                column_len_cnt += chunk_info_ptr->row_cnt_;
            }
        }

        status = segment_index_meta.GetFtInfo(ft_info_ptr);
        if (!status.ok()) {
            return status;
        }
        column_len_sum += ft_info_ptr->ft_column_len_sum_;
        column_len_cnt += ft_info_ptr->ft_column_len_cnt_;

        {
            SharedPtr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
            SharedPtr<MemoryIndexer> memory_indexer = mem_index == nullptr ? nullptr : mem_index->GetFulltextIndex();
            if (memory_indexer && memory_indexer->GetDocCount() != 0) {
                SharedPtr<InMemIndexSegmentReader> segment_reader = MakeShared<InMemIndexSegmentReader>(segment_id, memory_indexer.get());
                segment_readers_.push_back(std::move(segment_reader));
                // for loading column length file
                memory_indexer_ = memory_indexer;
                column_len_sum += memory_indexer_->GetColumnLengthSum();
                column_len_cnt += memory_indexer_->GetDocCount();
            }
        }
        segment_index_ft_infos_.emplace(segment_id, std::move(ft_info_ptr));
    }
    if (column_len_cnt != 0) {
        total_df_ = column_len_cnt;
        avg_column_length_ = static_cast<float>(column_len_sum) / column_len_cnt;
    }
    return Status::OK();
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

Pair<u64, float> ColumnIndexReader::GetTotalDfAndAvgColumnLength() {
    std::lock_guard lock(mutex_);
    if (total_df_ == 0) {
        u64 column_len_sum = 0;
        u32 column_len_cnt = 0;

        for (auto &[segment_id, segment_index_ft_info] : segment_index_ft_infos_) {
            column_len_sum += segment_index_ft_info->ft_column_len_sum_;
            column_len_cnt += segment_index_ft_info->ft_column_len_cnt_;
        }

        if (column_len_cnt != 0) {
            total_df_ = column_len_cnt;
            avg_column_length_ = static_cast<float>(column_len_sum) / column_len_cnt;
        }
    }
    return Pair<u64, float>(total_df_, avg_column_length_);
}

void ColumnIndexReader::InvalidateSegment(SegmentID segment_id) {
    for (auto iter = segment_readers_.begin(); iter != segment_readers_.end();) {
        if ((*iter)->segment_id() == segment_id) {
            iter = segment_readers_.erase(iter);
        } else {
            ++iter;
        }
    }
    for (auto iter = chunk_index_meta_infos_.begin(); iter != chunk_index_meta_infos_.end();) {
        if ((*iter).segment_id_ == segment_id) {
            iter = chunk_index_meta_infos_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void ColumnIndexReader::InvalidateChunk(SegmentID segment_id, ChunkID chunk_id) {
    for (auto iter = segment_readers_.begin(); iter != segment_readers_.end();) {
        if ((*iter)->segment_id() == segment_id && (*iter)->chunk_id() == chunk_id) {
            iter = segment_readers_.erase(iter);
        } else {
            ++iter;
        }
    }
    for (auto iter = chunk_index_meta_infos_.begin(); iter != chunk_index_meta_infos_.end();) {
        if ((*iter).segment_id_ == segment_id && (*iter).chunk_id_ == chunk_id) {
            iter = chunk_index_meta_infos_.erase(iter);
        } else {
            ++iter;
        }
    }
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

SharedPtr<IndexReader> TableIndexReaderCache::GetIndexReader(NewTxn *txn) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    SharedPtr<IndexReader> index_reader = MakeShared<IndexReader>();
    std::scoped_lock lock(mutex_);
    assert(cache_ts_ <= first_known_update_ts_);
    assert(first_known_update_ts_ == MAX_TIMESTAMP || first_known_update_ts_ <= last_known_update_ts_);
    if (first_known_update_ts_ != 0 && begin_ts >= cache_ts_ && begin_ts < first_known_update_ts_) [[likely]] {
        // no need to build, use cache
        index_reader->column_index_readers_ = cache_column_readers_;
        // result.column2analyzer_ = column2analyzer_;
    } else {
        FlatHashMap<u64, TxnTimeStamp, detail::Hash<u64>> cache_column_ts;
        index_reader->column_index_readers_ = MakeShared<FlatHashMap<u64, SharedPtr<Map<String, SharedPtr<ColumnIndexReader>>>, detail::Hash<u64>>>();
        // result.column2analyzer_ = MakeShared<Map<String, String>>();

        TableMeeta table_meta(db_id_str_, table_id_str_, txn);
        Vector<String> *index_id_strs = nullptr;
        {
            Status status = table_meta.GetIndexIDs(index_id_strs, nullptr);
            if (!status.ok()) {
                UnrecoverableError("GetIndexIDs failed");
            }
        }
        for (const String &index_id_str : *index_id_strs) {
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            auto [index_base, index_status] = table_index_meta.GetIndexBase();
            if (!index_status.ok()) {
                UnrecoverableError("Fail to get index definition");
            }
            if (index_base->index_type_ != IndexType::kFullText) {
                // non-fulltext index
                continue;
            }

            String column_name = index_base->column_name();
            auto [column_def, col_def_status] = table_index_meta.GetColumnDef();
            u64 column_id = column_def->id();
            if (index_reader->column_index_readers_->find(column_id) == index_reader->column_index_readers_->end()) {
                (*index_reader->column_index_readers_)[column_id] = MakeShared<Map<String, SharedPtr<ColumnIndexReader>>>();
            }
            auto column_index_map = (*index_reader->column_index_readers_)[column_id];

            // assert(table_index_entry->GetFulltextSegmentUpdateTs() <= last_known_update_ts_);
            if (auto &target_ts = cache_column_ts[column_id]; target_ts < begin_ts) {
                // need update result
                target_ts = begin_ts;
                const IndexFullText *index_full_text = reinterpret_cast<const IndexFullText *>(index_base.get());
                // update column2analyzer_
                // (*result.column2analyzer_)[column_name] = index_full_text->analyzer_;
                if (auto it = cache_column_ts_.find(column_id); it != cache_column_ts_.end() and it->second == begin_ts) {
                    // reuse cache
                    (*column_index_map)[index_id_str] = cache_column_readers_->at(column_id)->at(index_id_str);
                } else {
                    // new column_index_reader
                    auto column_index_reader = MakeShared<ColumnIndexReader>();
                    optionflag_t flag = index_full_text->flag_;
                    column_index_reader->Open(flag, table_index_meta);
                    column_index_reader->analyzer_ = index_full_text->analyzer_;
                    column_index_reader->column_name_ = column_name;
                    (*column_index_map)[index_id_str] = std::move(column_index_reader);
                }
            }
            if (begin_ts >= last_known_update_ts_) {
                // need to update cache
                cache_ts_ = last_known_update_ts_;
                first_known_update_ts_ = MAX_TIMESTAMP;
                last_known_update_ts_ = 0;
                cache_column_ts_ = std::move(cache_column_ts);
                cache_column_readers_ = index_reader->column_index_readers_;
                // column2analyzer_ = result.column2analyzer_;
            }
        }
    }
    return index_reader;
}

void TableIndexReaderCache::Invalidate() {
    std::scoped_lock lock(mutex_);
    first_known_update_ts_ = 0;
    last_known_update_ts_ = std::max(last_known_update_ts_, cache_ts_);
    cache_ts_ = 0;
    cache_column_ts_.clear();
    cache_column_readers_.reset();
    // column2analyzer_.reset();
}

void TableIndexReaderCache::InvalidateColumn(u64 column_id, const String &column_name) {
    std::scoped_lock lock(mutex_);
    cache_column_ts_.erase(column_id);
    if (cache_column_readers_.get() != nullptr) {
        cache_column_readers_->erase(column_id);
    }
    // if (column2analyzer_.get() != nullptr) {
    //     column2analyzer_->erase(column_name);
    // }
}

void TableIndexReaderCache::InvalidateSegmentColumn(u64 column_id, SegmentID segment_id) {
    std::scoped_lock lock(mutex_);
    if (!cache_column_readers_.get()) {
        return;
    }
    auto iter = cache_column_readers_->find(column_id);
    if (iter != cache_column_readers_->end()) {
        for (auto index_reader : (*iter->second)) {
            index_reader.second->InvalidateSegment(segment_id);
        }
    }
}

void TableIndexReaderCache::InvalidateChunkColumn(u64 column_id, SegmentID segment_id, ChunkID chunk_id) {
    std::scoped_lock lock(mutex_);
    if (!cache_column_readers_.get()) {
        return;
    }
    auto iter = cache_column_readers_->find(column_id);
    if (iter != cache_column_readers_->end()) {
        for (auto index_reader : (*iter->second)) {
            index_reader.second->InvalidateChunk(segment_id, chunk_id);
        }
    }
}

} // namespace infinity
