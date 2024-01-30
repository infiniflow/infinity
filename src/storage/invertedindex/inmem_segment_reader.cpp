module;

import stl;
import memory_pool;
import segment_posting;
import index_segment_reader;
import index_defines;
import posting_writer;
import column_indexer;

module inmem_index_segment_reader;

namespace infinity {
InMemIndexSegmentReader::InMemIndexSegmentReader(ColumnIndexer *column_indexer) : posting_table_(column_indexer->GetPostingTable()) {}

PostingWriter *InMemIndexSegmentReader::GetPostingWriter(const String &term) const {
    ColumnIndexer::PostingTable::Iterator iter = posting_table_->find(term);
    if (iter.valid())
        return iter.getData();
    else
        return nullptr;
}

bool InMemIndexSegmentReader::GetSegmentPosting(const String &term,
                                                docid_t base_doc_id,
                                                SegmentPosting &seg_posting,
                                                MemoryPool *session_pool) const {
    PostingWriter *posting_writer = GetPostingWriter(term);
    if (posting_writer == nullptr) {
        return false;
    }
    seg_posting.Init(base_doc_id, 0, posting_writer);
    return true;
}

docid_t InMemIndexSegmentReader::GetBaseDocId() const { return 0; }

} // namespace infinity