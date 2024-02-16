module;

export module disk_index_segment_reader;
import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_segment_reader;
import index_config;
import segment;
import dict_reader;
import file_reader;
import posting_list_format;
import local_file_system;

namespace infinity {
export class DiskIndexSegmentReader : public IndexSegmentReader {
public:
    DiskIndexSegmentReader(const Segment &segment, const InvertedIndexConfig &index_config);
    virtual ~DiskIndexSegmentReader();

    bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const override;

private:
    const Segment &segment_;
    SharedPtr<DictionaryReader> dict_reader_;
    SharedPtr<FileReader> posting_reader_;
    docid_t base_doc_id_{INVALID_DOCID};
    PostingFormatOption posting_format_option_;
    LocalFileSystem fs_;
};

} // namespace infinity