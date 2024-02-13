module;

module column_index_merger;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_list_format;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import column_index_iterator;
namespace infinity {
ColumnIndexMerger::ColumnIndexMerger(const InvertedIndexConfig &index_config, u64 column_id, MemoryPool *memory_pool, RecyclePool *buffer_pool)
    : index_config_(index_config), column_id_(column_id), memory_pool_(memory_pool), buffer_pool_(buffer_pool) {}

ColumnIndexMerger::~ColumnIndexMerger() {}

PostingMerger *ColumnIndexMerger::CreatePostingMerger(const Vector<Segment> &segments) {
    return new PostingMerger(memory_pool_, buffer_pool_, segments);
}

} // namespace infinity