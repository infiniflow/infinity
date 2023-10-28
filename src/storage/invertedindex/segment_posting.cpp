module;

import stl;
import byte_slice;
import posting_list_format;
import posting_writer;
import term_meta;
import byte_slice_reader;
import index_defines;
module segment_posting;

namespace infinity {
SegmentPosting::SegmentPosting(const PostingFormatOption &posting_option)
    : base_doc_id_(INVALID_DOCID), doc_count_(0), posting_writer_(nullptr), posting_option_(posting_option) {}

void SegmentPosting::Init(const SharedPtr<ByteSliceList> &slice_list, docid_t base_doc_id, u64 doc_count) {
    slice_list_ = slice_list;
    base_doc_id_ = base_doc_id;
    doc_count_ = doc_count;
    term_meta_ = GetCurrentTermMeta();
    posting_writer_ = nullptr;
}

void SegmentPosting::Init(docid_t base_doc_id, u64 doc_count) {
    base_doc_id_ = base_doc_id;
    doc_count_ = doc_count;
}

void SegmentPosting::Init(docid_t base_doc_id, u32 doc_count, PostingWriter *posting_writer) {
    base_doc_id_ = base_doc_id;
    doc_count_ = doc_count;
    posting_writer_ = posting_writer;
    GetTermMetaForRealtime(term_meta_);
}

TermMeta SegmentPosting::GetCurrentTermMeta() const {
    if (posting_writer_) {
        // in memory segment no truncate posting list
        return term_meta_;
    }

    if (slice_list_) {
        TermMeta tm;
        TermMetaLoader loader(posting_option_);
        ByteSliceReader reader(slice_list_.get());
        loader.Load(&reader, tm);
        return tm;
    }
    return term_meta_;
}

} // namespace infinity