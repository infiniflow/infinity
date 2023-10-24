#pragma once

#include "common/memory/memory_pool.h"
#include "format/posting_list_format.h"
#include "format/term_meta.h"
#include "in_doc_pos_iterator.h"
#include "segment_posting.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity {
class PostingIterator {
public:
    PostingIterator(const PostingFormatOption &posting_option, MemoryPool *session_pool);

    ~PostingIterator();

    TermMeta *GetTermMeta() const { return const_cast<TermMeta *>(&term_meta_); }

    docid_t SeekDoc(docid_t docId);

    void SeekPosition(pos_t pos, pos_t &result);

    void Reset() {}

private:
    PostingFormatOption posting_option_;
    MemoryPool *session_pool_;
    TermMeta term_meta_;
    docid_t last_doc_id_in_buffer_;
    docid_t current_doc_id_;
    docid_t *doc_buffer_cursor_;
    docid_t doc_buffer_[MAX_DOC_PER_RECORD];
    docid_t *doc_buffer_base;
    ttf_t current_ttf_;
    int32_t tf_buffer_cursor_;
    tf_t *tf_buffer_;
    docpayload_t *doc_payload_buffer_;

    // BufferedPostingDecoder *decoder_;
    bool in_doc_pos_iter_inited_;
    InDocPositionState state_;
    InDocPositionIterator *in_doc_pos_iterator_;
};
} // namespace infinity