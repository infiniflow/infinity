module;

export module posting_merger;

import stl;
import memory_pool;
import file_writer;
import posting_decoder;
import posting_list_format;
import index_defines;
import term_meta;
import column_index_iterator;
import segment_term_posting;

namespace infinity {

class PostingDumper;
export class PostingMerger {
public:
    PostingMerger(MemoryPool *memory_pool, RecyclePool *buffer_pool);

    ~PostingMerger();

    void Merge(const Vector<SegmentTermPosting *> &segment_term_postings);

    void Dump(const SharedPtr<FileWriter> &file_writer, TermMeta &term_meta);

    u32 GetDF();

    u32 GetTotalTF();

private:
    MemoryPool *memory_pool_{nullptr};
    RecyclePool *buffer_pool_{nullptr};
    PostingFormatOption format_option_;
    SharedPtr<PostingDumper> posting_dumper_;
    df_t df_;
    ttf_t ttf_;
};
} // namespace infinity