module;
#include <filesystem>

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
import parser;
import column_vector;
module indexer;

namespace infinity {
Indexer::Indexer() {}

Indexer::~Indexer() {
    if (posting_table_) {
        for (PostingTable::iterator it = posting_table_->begin(); it != posting_table_->end(); ++it) {
            PostingWriter *posting_writer = it->second;
            posting_writer->~PostingWriter();
            buffer_pool_->Deallocate((void *)posting_writer, sizeof(PostingWriter));
        }
        posting_table_->clear();
    }
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
    posting_table_ = new PostingTable;
}

void Indexer::Add(DataBlock *data_block) {
    Vector<RowID> row_ids;
    Vector<SharedPtr<ColumnVector>> column_vectors;
    for (SizeT i = 0; i < data_block->column_count(); i++) {
        SharedPtr<ColumnVector> column_vector = data_block->column_vectors[i];
        if (column_vector->data_type()->type() == LogicalType::kRowID) {
            row_ids.resize(column_vector->Size());
            Memcpy(row_ids.data(), column_vector->data(), column_vector->Size() * sizeof(RowID));
            break;
        } else {
            column_vectors.push_back(column_vector);
        }
    }
}

void Indexer::Commit() {}

void Indexer::DoAddPosting(const String &term) {
    PostingTable::iterator it = posting_table_->find(term);
    if (it == posting_table_->end()) {
        PostingWriter *posting_writer = new (buffer_pool_->Allocate(sizeof(PostingWriter)))
            PostingWriter(byte_slice_pool_.get(), buffer_pool_.get(), index_config_.GetPostingFormatOption());
        posting_table_->emplace(term, posting_writer);
    }
}

SharedPtr<IndexSegmentReader> Indexer::CreateInMemSegmentReader() { return MakeShared<InMemIndexSegmentReader>(); }
} // namespace infinity
