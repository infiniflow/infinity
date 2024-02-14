module;

export module segment_term_posting;

import stl;
import memory_pool;
import file_writer;
import posting_decoder;
import posting_list_format;
import index_defines;
import term_meta;
import segment;
import column_index_iterator;
import index_config;

namespace infinity {
// Utility class for posting merging
export class SegmentTermPosting {
public:
    SegmentTermPosting();

    SegmentTermPosting(segmentid_t segment_id, docid_t base_doc);

    docid_t GetBaesDocId() { return base_doc_id_; }

    bool HasNext();

    PostingDecoder *GetPostingDecoder() { return posting_decoder_; }

public:
    segmentid_t segment_id_;
    docid_t base_doc_id_;
    String term_;
    PostingDecoder *posting_decoder_{nullptr};
    SharedPtr<ColumnIndexIterator> column_index_iterator_;
};

class SegmentTermPostingComparator {
public:
    bool operator()(const SegmentTermPosting *item1, const SegmentTermPosting *item2) {
        int ret = item1->term_.compare(item2->term_);
        if (ret != 0)
            return ret > 0;
        return item1->segment_id_ > item2->segment_id_;
    }
};

export class SegmentTermPostingQueue {
public:
    SegmentTermPostingQueue(const InvertedIndexConfig &index_config, u64 column_id);

    ~SegmentTermPostingQueue();

    bool Empty() const { return segment_term_postings_.empty(); }

    void Init(const Vector<Segment> &segments);

    const Vector<SegmentTermPosting *> &GetCurrentMerging(String &term);

    void MoveToNextTerm();

private:
    SharedPtr<ColumnIndexIterator> CreateIndexIterator(segmentid_t segment_id);

private:
    using PriorityQueue = Heap<SegmentTermPosting *, SegmentTermPostingComparator>;
    const InvertedIndexConfig &index_config_;
    u64 column_id_;
    PriorityQueue segment_term_postings_;
    Vector<SegmentTermPosting *> merging_term_postings_;
};

} // namespace infinity