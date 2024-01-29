module;

module disk_index_segment_reader;
import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_segment_reader;
import index_config;
import segment;
import file_reader;
import dict_reader;
import posting_list_format;

namespace infinity {

DiskIndexSegmentReader::DiskIndexSegmentReader(const String &root_path,
                                               const Segment &segment,
                                               const InvertedIndexConfig &index_config,
                                               const SharedPtr<DictionaryReader> &dict_reader)
    : path_(root_path), dict_reader_(dict_reader) {
    posting_format_option_ = index_config.GetPostingFormatOption();
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {}

bool DiskIndexSegmentReader::GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const {
    u64 dict_value;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, dict_value))
        return false;
    /// TODO
    return true;
}

docid_t DiskIndexSegmentReader::GetBaseDocId() const { return 0; }

} // namespace infinity
