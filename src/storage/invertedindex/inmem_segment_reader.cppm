module;

export module inmem_index_segment_reader;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import index_defines;
import posting_writer;
import column_indexer;
import memory_indexer;

export module inmem_index_segment_reader;

namespace infinity {
export class InMemIndexSegmentReader : public IndexSegmentReader {
public:
    InMemIndexSegmentReader(MemoryIndexer *column_indexer);
    virtual ~InMemIndexSegmentReader() = default;

    docid_t GetBaseDocId() const override;

    bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const override;

private:
    PostingWriter *GetPostingWriter(const String &term) const;

    MemoryIndexer::PostingTable *posting_table_{nullptr};
};

} // namespace infinity