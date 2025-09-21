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

module infinity_core:column_index_reader.impl;

import :column_index_reader;
import :segment_posting;
import :index_segment_reader;
import :posting_iterator;
import :index_defines;
import :disk_index_segment_reader;
import :inmem_index_segment_reader;
import :memory_indexer;
import :dict_reader;
import :posting_list_format;
import :infinity_exception;
import :index_base;
import :index_full_text;
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

import std;
import third_party;

import create_index_info;
import internal_types;

namespace infinity {

ColumnIndexReader::~ColumnIndexReader() = default;

Status ColumnIndexReader::Open(optionflag_t flag, TableIndexMeeta &table_index_meta) {
    flag_ = flag;

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
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
        std::shared_ptr<std::string> index_dir = segment_index_meta.GetSegmentIndexDir();
        std::shared_ptr<SegmentIndexFtInfo> ft_info_ptr;
        Status status = segment_index_meta.GetFtInfo(ft_info_ptr);
        if (!status.ok()) {
            return status;
        }

        std::vector<ChunkID> *chunk_ids_ptr = nullptr;
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
            std::shared_ptr<DiskIndexSegmentReader> segment_reader = std::make_shared<DiskIndexSegmentReader>(segment_id,
                                                                                                              chunk_id,
                                                                                                              *index_dir,
                                                                                                              chunk_info_ptr->base_name_,
                                                                                                              chunk_info_ptr->base_row_id_,
                                                                                                              flag);
            segment_readers_.push_back(std::move(segment_reader));

            BufferObj *index_buffer = nullptr;
            status = chunk_index_meta.GetIndexBuffer(index_buffer);
            if (!status.ok()) {
                return status;
            }

            chunk_index_meta_infos_.emplace_back(
                ColumnReaderChunkInfo{index_buffer, chunk_info_ptr->base_row_id_, u32(chunk_info_ptr->row_cnt_), chunk_id, segment_id});
            column_len_sum += chunk_info_ptr->term_cnt_;
            column_len_cnt += chunk_info_ptr->row_cnt_;
        }

        {
            std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
            std::shared_ptr<MemoryIndexer> memory_indexer = mem_index == nullptr ? nullptr : mem_index->GetFulltextIndex();
            if (memory_indexer && memory_indexer->GetDocCount() != 0) {
                std::shared_ptr<InMemIndexSegmentReader> segment_reader = std::make_shared<InMemIndexSegmentReader>(segment_id, memory_indexer.get());
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

std::unique_ptr<PostingIterator> ColumnIndexReader::Lookup(const std::string &term, bool fetch_position) {
    std::shared_ptr<std::vector<SegmentPosting>> seg_postings = std::make_shared<std::vector<SegmentPosting>>();
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
    auto iter = std::make_unique<PostingIterator>(flag_);
    u32 state_pool_size = 0; // TODO
    iter->Init(std::move(seg_postings), state_pool_size);
    return iter;
}

std::pair<u64, float> ColumnIndexReader::GetTotalDfAndAvgColumnLength() { return std::pair<u64, float>(total_df_, avg_column_length_); }

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

std::shared_ptr<IndexReader> TableIndexReaderCache::GetIndexReader(NewTxn *txn) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    std::shared_ptr<IndexReader> index_reader = std::make_shared<IndexReader>();
    std::scoped_lock lock(mutex_);
    if (begin_ts >= cache_ts_) [[likely]] {
        // no need to build, use cache
        LOG_INFO(fmt::format("DEBUG: Using cached index readers for table_id: '{}'", table_id_str_));
        index_reader->column_index_readers_ = cache_column_readers_;
        return index_reader;
    }

    index_reader->column_index_readers_ =
        std::make_shared<FlatHashMap<u64, std::shared_ptr<std::map<std::string, std::shared_ptr<ColumnIndexReader>>>, detail::Hash<u64>>>();

    TableMeeta table_meta(db_id_str_, table_id_str_, table_name_, txn);
    std::vector<std::string> *index_id_strs = nullptr;
    std::vector<std::string> *index_name_strs = nullptr;
    {
        Status status = table_meta.GetIndexIDs(index_id_strs, &index_name_strs);
        if (!status.ok()) {
            UnrecoverableError("GetIndexIDs failed");
        }
    }
    size_t index_count = index_id_strs->size();
    for (size_t idx = 0; idx < index_count; ++idx) {
        const std::string &index_id_str = index_id_strs->at(idx);
        const std::string &index_name_str = index_name_strs->at(idx);

        TableIndexMeeta table_index_meta(index_id_str, index_name_str, table_meta);
        auto [index_base, index_status] = table_index_meta.GetIndexBase();
        if (!index_status.ok()) {
            UnrecoverableError("Fail to get index definition");
        }
        if (index_base->index_type_ != IndexType::kFullText) {
            // non-fulltext index
            continue;
        }

        std::string column_name = index_base->column_name();
        auto [column_def, col_def_status] = table_index_meta.GetColumnDef();
        u64 column_id = column_def->id();
        if (index_reader->column_index_readers_->find(column_id) == index_reader->column_index_readers_->end()) {
            (*index_reader->column_index_readers_)[column_id] = std::make_shared<std::map<std::string, std::shared_ptr<ColumnIndexReader>>>();
        }
        auto column_index_map = (*index_reader->column_index_readers_)[column_id];

        // assert(table_index_entry->GetFulltextSegmentUpdateTs() <= last_known_update_ts_);
        const IndexFullText *index_full_text = reinterpret_cast<const IndexFullText *>(index_base.get());
        // new column_index_reader
        auto column_index_reader = std::make_shared<ColumnIndexReader>();
        optionflag_t flag = index_full_text->flag_;
        column_index_reader->Open(flag, table_index_meta);
        column_index_reader->analyzer_ = index_full_text->analyzer_;
        column_index_reader->column_name_ = column_name;
        (*column_index_map)[index_id_str] = std::move(column_index_reader);
    }

    if (cache_ts_ == UNCOMMIT_TS || begin_ts > cache_ts_) {
        // need to update cache
        cache_ts_ = begin_ts;
        cache_column_readers_ = index_reader->column_index_readers_;
    }
    return index_reader;
}

void TableIndexReaderCache::Invalidate() {
    std::scoped_lock lock(mutex_);
    cache_ts_ = UNCOMMIT_TS;
    cache_column_readers_.reset();
}

} // namespace infinity
