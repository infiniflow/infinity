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

#include <filesystem>

module indexer;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import index_config;
import inmem_index_segment_reader;
import posting_writer;
import block_entry;
import block_column_entry;
import buffer_manager;
import segment;

import column_vector;
import third_party;
import column_indexer;

import logical_type;
import internal_types;
#include <unistd.h>

namespace infinity {

Indexer::Indexer() {
    thd_commit_ = std::thread([this] {
        while (!stopped_.load()) {
            sleep(2);
            this->Commit();
        }
    });
    thd_commit_.detach();
}

Indexer::~Indexer() {
    stopped_.store(true);
    thd_commit_.join();
    thread_pool_.stop();
}

void Indexer::Open(const InvertedIndexConfig &index_config, const String &directory) {
    index_config_ = index_config;
    String index_name = index_config_.GetIndexName();
    Path path = Path(directory) / index_name;
    directory_ = path.string();
    std::error_code ec;
    bool path_exists = std::filesystem::exists(path);
    if (!path_exists) {
        std::filesystem::create_directories(path, ec);
    }
    byte_slice_pool_.reset(new MemoryPool);
    buffer_pool_.reset(new RecyclePool);

    index_config_.GetColumnIDs(column_ids_);
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        String index_dir = GetDirectory();
        Path path = Path(index_dir) / std::to_string(column_id);
        index_dir = path.string();
        column_indexers_[column_id] = MakeUnique<ColumnIndexer>(column_id, index_dir, index_config_, byte_slice_pool_, buffer_pool_, thread_pool_);
    }

    id_generator_ = MakeShared<IDGenerator>();
    id_generator_->SetCurrentSegmentID(index_config_.GetLastSegmentID());

    index_config_.GetSegments(segments_);

    AddSegment();
    dump_ref_count_ = column_ids_.size();

    u32 num_threads = index_config.GetIndexingParallelism();
    thread_pool_.resize(num_threads);
}

void Indexer::AddSegment() {
    segmentid_t next_segment_id = GetNextSegmentID();
    segments_.push_back(Segment(Segment::BUILDING));
    Segment &segment = segments_.back();
    segment.SetSegmentId(next_segment_id);
    active_segment_.store(&segment, std::memory_order_acq_rel);
}

void Indexer::UpdateSegment(RowID row_id, u64 inc_count) {
    std::shared_lock<std::shared_mutex> lock(flush_mutex_);
    auto *ctx = active_segment_.load(std::memory_order_relaxed);
    if (ctx->GetBaseDocId() == INVALID_DOCID) {
        ctx->SetBaseDocId(RowID2DocID(row_id));
    }
    ctx->IncDocCount(inc_count);
}

void Indexer::BatchInsert(const BlockEntry *block_entry, u32 row_offset, u32 row_count, BufferManager *buffer_mgr) {
    if (row_count <= 0)
        return;
    RowID row_id_begin(block_entry->segment_id(), block_entry->block_id() * block_entry->row_capacity() + row_offset);
    docid_t doc_id_begin = RowID2DocID(row_id_begin);
    auto *ctx = active_segment_.load(std::memory_order_relaxed);
    if (ctx->GetBaseDocId() == INVALID_DOCID) {
        ctx->SetBaseDocId(doc_id_begin);
    } else {
        if (ctx->GetNextDocId() != doc_id_begin) {
            // doc id gap
            AddSegment();
            ctx = active_segment_.load(std::memory_order_relaxed);
            ctx->SetBaseDocId(doc_id_begin);
        }
    }

    for (auto &[column_id, column_indexer] : column_indexers_) {
        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
        ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);
        column_indexers_[column_id]->Insert(column_vector, row_offset, row_count, row_id_begin);
    }
    ctx->IncDocCount(row_count);
}

void Indexer::Commit() {
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Commit();
    }
}

void Indexer::Dump() {
    auto *flush_segment = active_segment_.load(std::memory_order_relaxed);
    AddSegment();
    flush_segment->SetSegmentStatus(Segment::DUMPING);

    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Dump();
    }
    flush_segment->SetSegmentStatus(Segment::BUILT);
}

SharedPtr<InMemIndexSegmentReader> Indexer::CreateInMemSegmentReader(u64 column_id) {
    return MakeShared<InMemIndexSegmentReader>(column_indexers_[column_id]->GetMemoryIndexer());
}

void Indexer::GetSegments(Vector<Segment> &segments) { segments_ = segments; }
} // namespace infinity
