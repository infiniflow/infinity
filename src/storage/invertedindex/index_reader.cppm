module;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import posting_iterator;
import index_defines;
import index_config;
import segment;
import disk_index_segment_reader;
import inmem_index_segment_reader;
export module index_reader;

namespace infinity {
export class IndexReader {
public:
    IndexReader() = default;
    virtual ~IndexReader() = default;

    void Open(const InvertedIndexConfig &index_config);

    PostingIterator *Lookup(const String &term, MemoryPool *session_pool);

    void GetSegments(const String &directory, Vector<Segment> &segments);

    bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) { return false; }

private:
    SharedPtr<DiskIndexSegmentReader> CreateDiskSegmentReader(const Segment &segment);

    SharedPtr<InMemIndexSegmentReader> CreateInMemSegmentReader(Segment &segment);

private:
    String root_dir_;
    InvertedIndexConfig index_config_;
    Vector<SharedPtr<IndexSegmentReader>> segment_readers_;
    Vector<docid_t> base_doc_ids_;
};

} // namespace infinity