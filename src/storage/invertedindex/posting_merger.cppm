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

namespace infinity {
export class SegmentTermPosting {
public:
    SegmentTermPosting();
    SegmentTermPosting(segmentid_t segment_id, docid_t base_doc);

    docid_t GetBaesDocId() { return base_doc_id_; }

    bool HasNext();

    PostingDecoder *GetPostingDecoder() { return posting_decoder_; }

private:
    segmentid_t segment_id_;
    docid_t base_doc_id_;
    String term_;
    PostingDecoder *posting_decoder_{nullptr};
    SharedPtr<ColumnIndexIterator> column_index_iterator_;
};

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