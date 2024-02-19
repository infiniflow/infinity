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


namespace infinity {

Indexer::Indexer() {}

Indexer::~Indexer() {}

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
        column_indexers_[column_id] = MakeUnique<ColumnIndexer>(this, column_id, index_config_, byte_slice_pool_, buffer_pool_);
    }

    id_generator_ = MakeShared<IDGenerator>();
    id_generator_->SetCurrentSegmentID(index_config_.GetLastSegmentID());

    index_config_.GetSegments(segments_);

    AddSegment();
    dump_ref_count_ = column_ids_.size();
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
            // TODO: doc id is not continuous
        }
    }

    for (auto &[column_id, column_indexer] : column_indexers_) {
        BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
        ColumnVector column_vector = block_column_entry->GetColumnVector(buffer_mgr);
        column_indexers_[column_id]->Insert(column_vector, row_id_begin);
    }
    ctx->IncDocCount(row_count);
}

void Indexer::Insert(RowID row_id, String &data) {
    UpdateSegment(row_id);
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Insert(row_id, data);
    }
}

void Indexer::Commit() {
    // Commit is called in a dedicate thread periodically
    {
        // PreCommit is used to switch internal inverters such that all
        // data after the Commit is called from other threads will not
        // be out of sync
        std::unique_lock<std::shared_mutex> lock(flush_mutex_);
        for (SizeT i = 0; i < column_ids_.size(); ++i) {
            u64 column_id = column_ids_[i];
            column_indexers_[column_id]->PreCommit();
        }
    }
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Commit();
    }
}

void Indexer::Dump() {
    auto *flush_segment = active_segment_.load(std::memory_order_relaxed);
    AddSegment();
    flush_segment->SetSegmentStatus(Segment::DUMPLING);

    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Dump();
    }
    flush_segment->SetSegmentStatus(Segment::BUILT);
}

SharedPtr<InMemIndexSegmentReader> Indexer::CreateInMemSegmentReader(u64 column_id) {
    return MakeShared<InMemIndexSegmentReader>(column_indexers_[column_id]->GetMemoryIndexer());
}

void Indexer::TryDump() {
    dump_ref_count_.fetch_sub(1, std::memory_order_acq_rel);

    if (dump_ref_count_ == 0) {
        // TODO, using a global resource manager to control the memory quota
        if (byte_slice_pool_->GetUsedBytes() >= index_config_.GetMemoryQuota()) {
            Dump();
        }
        dump_ref_count_ = column_ids_.size();
    }
}

void Indexer::GetSegments(Vector<Segment> &segments) { segments_ = segments; }
} // namespace infinity
