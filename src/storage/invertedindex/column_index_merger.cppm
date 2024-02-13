module;

export module column_index_merger;

import stl;
import memory_pool;
import byte_slice;
import byte_slice_reader;
import file_reader;
import posting_decoder;
import posting_merger;
import posting_list_format;
import bitmap;
import index_defines;
import term_meta;
import segment;
import index_config;
import column_index_iterator;
namespace infinity {
export class ColumnIndexMerger {
public:
    ColumnIndexMerger(const InvertedIndexConfig &index_config, u64 column_id);
    ~ColumnIndexMerger();

private:
    PostingMerger *CreatePostingMerger(const Vector<Segment> &segments);

    InvertedIndexConfig index_config_;
    u64 column_id_;
};
} // namespace infinity