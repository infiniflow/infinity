module;

export module posting_merger;

import stl;
import memory_pool;
import file_writer;
import posting_decoder;
import posting_list_format;
import index_defines;
import term_meta;
import segment;
import column_index_iterator;
import segment_term_posting;

namespace infinity {

class PostingDumper;
export class PostingMerger {
public:
    PostingMerger(MemoryPool *memory_pool, RecyclePool *buffer_pool, const Vector<Segment> &segments);
    ~PostingMerger();

    void Merge(const Vector<SegmentTermPosting *> &segment_term_postings);

private:
    MemoryPool *memory_pool_{nullptr};
    RecyclePool *buffer_pool_{nullptr};
    PostingFormatOption format_option_;
    SharedPtr<PostingDumper> posting_dumper_;
    df_t df_;
    ttf_t ttf_;
};
} // namespace infinity