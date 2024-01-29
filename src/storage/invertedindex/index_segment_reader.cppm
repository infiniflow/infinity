module;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import segment;
export module index_segment_reader;

namespace infinity {
export class IndexSegmentReader {
public:
    IndexSegmentReader();
    virtual ~IndexSegmentReader() {}

    virtual bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const = 0;
};

} // namespace infinity