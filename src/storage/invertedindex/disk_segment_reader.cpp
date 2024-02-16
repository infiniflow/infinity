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
import term_meta;
import byte_slice;
import posting_list_format;

namespace infinity {

DiskIndexSegmentReader::DiskIndexSegmentReader(const Segment &segment, const InvertedIndexConfig &index_config) : segment_(segment) {
    posting_format_option_ = index_config.GetPostingFormatOption();

    String root_dir = index_config.GetIndexName();
    Path path = Path(root_dir) / std::to_string(segment_.GetSegmentId());
    String dict_file = path.string();
    dict_file.append(DICT_SUFFIX);
    dict_reader_ = MakeShared<DictionaryReader>(dict_file, index_config.GetPostingFormatOption());
    String posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);
    posting_reader_ = MakeShared<FileReader>(fs_, posting_file, 1024);
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {}

bool DiskIndexSegmentReader::GetSegmentPosting(const String &term, docid_t base_doc_id, SegmentPosting &seg_posting, MemoryPool *session_pool) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta))
        return false;
    posting_reader_->Seek(term_meta.doc_start_);
    u64 file_length = term_meta.pos_start_ - term_meta.doc_start_;
    ByteSlice *slice = ByteSlice::CreateSlice(file_length, session_pool);
    posting_reader_->Read((char *)slice->data_, file_length);
    SharedPtr<ByteSliceList> byte_slice_list = MakeShared<ByteSliceList>(slice, session_pool);
    seg_posting.Init(byte_slice_list, base_doc_id, term_meta.doc_freq_);
    return true;
}

docid_t DiskIndexSegmentReader::GetBaseDocId() const { return 0; }

} // namespace infinity
