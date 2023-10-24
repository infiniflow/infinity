#include "posting_iterator.h"

namespace infinity {

PostingIterator::PostingIterator(const PostingFormatOption &posting_option, MemoryPool *session_pool)
    : posting_option_(posting_option), session_pool_(session_pool), last_doc_id_in_buffer_(INVALID_DOCID - 1), current_doc_id_(INVALID_DOCID),
      doc_buffer_cursor_(NULL), current_ttf_(0), tf_buffer_cursor_(0), tf_buffer_(NULL), doc_payload_buffer_(NULL), in_doc_pos_iter_inited_(false),
      in_doc_pos_iterator_(nullptr) {
    tf_buffer_ = (tf_t *)((session_pool_)->Allocate(sizeof(tf_t) * MAX_DOC_PER_RECORD));
    doc_payload_buffer_ = (docpayload_t *)((session_pool_)->Allocate(sizeof(docpayload_t) * MAX_DOC_PER_RECORD));
    doc_buffer_base = doc_buffer_;
}

PostingIterator::~PostingIterator() {
    if (tf_buffer_) {
        session_pool_->Deallocate((void *)tf_buffer_, sizeof(tf_t) * MAX_DOC_PER_RECORD);
    }
    if (doc_payload_buffer_) {
        session_pool_->Deallocate((void *)doc_payload_buffer_, sizeof(docpayload_t) * MAX_DOC_PER_RECORD);
    }
}
docid_t PostingIterator::SeekDoc(docid_t docId) {
    docid_t ret = INVALID_DOCID;
    return ret;
}

void PostingIterator::SeekPosition(pos_t pos, pos_t &result) {
    if (!in_doc_pos_iter_inited_) {
        in_doc_pos_iter_inited_ = true;
    }
    in_doc_pos_iterator_->SeekPosition(pos, result);
}

} // namespace infinity