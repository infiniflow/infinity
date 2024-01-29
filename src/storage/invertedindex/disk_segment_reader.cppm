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

namespace infinity {
export class DiskIndexSegmentReader : public IndexSegmentReader {
public:
    DiskIndexSegmentReader(const String &root_path,
                           const Segment &segment,
                           const InvertedIndexConfig &index_config,
                           const SharedPtr<DictionaryReader> &dict_reader);
    virtual ~DiskIndexSegmentReader();

    docid_t GetBaseDocId() const override;

    bool GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const override;

private:
    String path_;
    SharedPtr<DictionaryReader> dict_reader_;
    SharedPtr<FileReader> posting_reader_;
    PostingFormatOption posting_format_option_;
};

} // namespace infinity