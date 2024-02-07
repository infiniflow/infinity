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
import data_block;
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
    segmentid_t next_segment_id = GetNextSegmentID();
    segments_.push_back(Segment(Segment::BUILDING));
    Segment &segment = segments_.back();
    segment.SetSegmentId(next_segment_id);

    dump_ref_count_ = column_ids_.size();
}

void Indexer::Add(DataBlock *data_block) {
    Vector<RowID> row_ids;
    Vector<SharedPtr<ColumnVector>> column_vectors;
    for (SizeT i = 0; i < data_block->column_count(); ++i) {
        SharedPtr<ColumnVector> column_vector = data_block->column_vectors[i];
        if (column_vector->data_type()->type() == LogicalType::kRowID) {
            row_ids.resize(column_vector->Size());
            std::memcpy(row_ids.data(), column_vector->data(), column_vector->Size() * sizeof(RowID));
            break;
        } else {
            column_vectors.push_back(column_vector);
        }
    }
    RowID start_row_id = row_ids[0]; // TODO: interface requires to be optimized
    UpdateSegment(start_row_id);
    for (SizeT i = 0; i < column_vectors.size(); ++i) {
        /// TODO column_id ?
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Insert(column_vectors[i], start_row_id);
    }
}

void Indexer::Insert(RowID row_id, String &data) {
    UpdateSegment(row_id);
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Insert(row_id, data);
    }
}

void Indexer::Commit() {
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Commit();
    }
}

void Indexer::Dump() {
    for (SizeT i = 0; i < column_ids_.size(); ++i) {
        u64 column_id = column_ids_[i];
        column_indexers_[column_id]->Dump();
    }
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
