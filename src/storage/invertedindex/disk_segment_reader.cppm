module;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_segment_reader;
import index_config;
import segment;
export module disk_index_segment_reader;

namespace infinity {
export class DiskIndexSegmentReader : public IndexSegmentReader {
public:
    DiskIndexSegmentReader(const String &root_path, const Segment &segment);
    virtual ~DiskIndexSegmentReader() = default;

    bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const override {
        return false;
    }

private:
    String path_;
};

} // namespace infinity