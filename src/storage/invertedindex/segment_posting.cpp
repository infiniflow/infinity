#include "segment_posting.h"

namespace infinity {
SegmentPosting::SegmentPosting(const PostingFormatOption &posting_option)
    : base_doc_id_(INVALID_DOCID), doc_count_(0), posting_writer_(nullptr), posting_option_(posting_option) {}

void SegmentPosting::Init(const std::shared_ptr<ByteSliceList> &slice_list, docid_t base_doc_id, uint64_t doc_count) {
    slice_list_ = slice_list;
    base_doc_id_ = base_doc_id;
    doc_count_ = doc_count;
    term_meta_ = GetCurrentTermMeta();
    posting_writer_ = NULL;
}

void SegmentPosting::Init(docid_t base_doc_id, uint64_t doc_count) {
    base_doc_id_ = base_doc_id;
    doc_count_ = doc_count;
}

void SegmentPosting::Init(docid_t base_doc_id, uint32_t doc_count, PostingWriter *posting_writer) {
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