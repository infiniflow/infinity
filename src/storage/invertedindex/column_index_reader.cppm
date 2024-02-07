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
export module column_index_reader;

namespace infinity {
class Indexer;
export class ColumnIndexReader {
public:
    ColumnIndexReader(u64 column_id, Indexer *indexer);
    virtual ~ColumnIndexReader() = default;

    void Open(const InvertedIndexConfig &index_config);

    PostingIterator *Lookup(const String &term, MemoryPool *session_pool);

    bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) { return false; }

private:
    SharedPtr<DiskIndexSegmentReader> CreateDiskSegmentReader(const Segment &segment);

    SharedPtr<InMemIndexSegmentReader> CreateInMemSegmentReader(Segment &segment);

private:
    u64 column_id_;
    String root_dir_;
    InvertedIndexConfig index_config_;
    Indexer *indexer_;
    Vector<SharedPtr<IndexSegmentReader>> segment_readers_;
    Vector<docid_t> base_doc_ids_;
};

} // namespace infinity