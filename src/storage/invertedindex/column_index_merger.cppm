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
import segment_term_posting;

namespace infinity {
export class ColumnIndexMerger {
public:
    ColumnIndexMerger(const InvertedIndexConfig &index_config, u64 column_id, MemoryPool *memory_pool, RecyclePool *buffer_pool);
    ~ColumnIndexMerger();

    void Merge(const Vector<Segment> &segments);

private:
    SharedPtr<PostingMerger> CreatePostingMerger(const Vector<Segment> &segments);

    void MergeTerm(const String &term, const Vector<SegmentTermPosting *> &merging_term_postings, const Vector<Segment> &segments);

    InvertedIndexConfig index_config_;
    u64 column_id_;
    MemoryPool *memory_pool_{nullptr};
    RecyclePool *buffer_pool_{nullptr};
};
} // namespace infinity