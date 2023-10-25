#pragma once

#include "common/memory/memory_pool.h"
#include "format/posting_list_format.h"
#include "format/term_meta.h"
#include "in_doc_pos_iterator.h"
#include "multi_posting_decoder.h"
#include "segment_posting.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity {
class PostingIterator {
public:
    PostingIterator(const PostingFormatOption &posting_option, MemoryPool *session_pool);

    ~PostingIterator();

    bool Init(const std::shared_ptr<std::vector<SegmentPosting>> &seg_postings, const uint32_t state_pool_size);

    void Reset();

    TermMeta *GetTermMeta() const { return const_cast<TermMeta *>(&term_meta_); }

    docid_t SeekDoc(docid_t docId);

    void SeekPosition(pos_t pos, pos_t &result);

    ttf_t GetCurrentTTF() {
        if (posting_option_.HasTfList()) {
            DecodeTFBuffer();
            int32_t offset = GetDocOffsetInBuffer();
            for (; tf_buffer_cursor_ < offset; ++tf_buffer_cursor_) {
                current_ttf_ += tf_buffer_[tf_buffer_cursor_];
            }
        }
        return current_ttf_;
    }

    bool HasPosition() const { return posting_option_.HasPositionList(); }

private:
    uint32_t GetCurrentSeekedDocCount() const { return posting_decoder_->InnerGetSeekedDocCount() + (GetDocOffsetInBuffer() + 1); }

    int32_t GetDocOffsetInBuffer() const { return doc_buffer_cursor_ - doc_buffer_base_ - 1; }

    void DecodeTFBuffer() {
        if (posting_option_.HasTfList()) {
            if (posting_decoder_->DecodeCurrentTFBuffer(tf_buffer_)) {
                tf_buffer_cursor_ = 0;
            }
        }
    }

    void DecodeDocPayloadBuffer() {
        if (posting_option_.HasDocPayload()) {
            posting_decoder_->DecodeCurrentDocPayloadBuffer(doc_payload_buffer_);
        }
    }

    void MoveToCurrentDoc();

    tf_t InnerGetTF() {
        if (posting_option_.HasTfList()) {
            DecodeTFBuffer();
            return tf_buffer_[GetDocOffsetInBuffer()];
        }
        return 0;
    }

private:
    std::shared_ptr<std::vector<SegmentPosting>> segment_postings_;
    PostingFormatOption posting_option_;
    MemoryPool *session_pool_;
    TermMeta term_meta_;
    docid_t last_doc_id_in_buffer_;
    docid_t current_doc_id_;
    docid_t *doc_buffer_cursor_;
    docid_t doc_buffer_[MAX_DOC_PER_RECORD];
    docid_t *doc_buffer_base_;
    ttf_t current_ttf_;
    int32_t tf_buffer_cursor_;
    tf_t *tf_buffer_;
    docpayload_t *doc_payload_buffer_;
    MultiPostingDecoder *posting_decoder_;
    bool need_move_to_current_doc_;
    bool in_doc_pos_iter_inited_;
    InDocPositionState state_;
    InDocPositionIterator *in_doc_pos_iterator_;
};

} // namespace infinity