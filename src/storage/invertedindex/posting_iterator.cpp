module;

#include "common/utility/builtin.h"

import stl;
import memory_pool;
import byte_slice_reader;
import posting_list_format;
import term_meta;
import in_doc_pos_iterator;
import multi_posting_decoder;
import segment_posting;
import in_doc_pos_state;
import index_defines;
import internal_types;

module posting_iterator;

namespace infinity {

PostingIterator::PostingIterator(const PostingFormatOption &posting_option, MemoryPool *session_pool)
    : posting_option_(posting_option), session_pool_(session_pool), last_doc_id_in_buffer_(INVALID_ROWID), current_row_id_(INVALID_ROWID),
      doc_buffer_cursor_(nullptr), current_ttf_(0), tf_buffer_cursor_(0), tf_buffer_(nullptr), doc_payload_buffer_(nullptr),
      posting_decoder_(nullptr), need_move_to_current_doc_(false), in_doc_pos_iter_inited_(false), in_doc_pos_iterator_(nullptr) {
    tf_buffer_ = (tf_t *)((session_pool_)->Allocate(sizeof(tf_t) * MAX_DOC_PER_RECORD));
    doc_payload_buffer_ = (docpayload_t *)((session_pool_)->Allocate(sizeof(docpayload_t) * MAX_DOC_PER_RECORD));
    doc_buffer_base_ = doc_buffer_;
}

PostingIterator::~PostingIterator() {
    if (tf_buffer_) {
        session_pool_->Deallocate((void *)tf_buffer_, sizeof(tf_t) * MAX_DOC_PER_RECORD);
    }
    if (doc_payload_buffer_) {
        session_pool_->Deallocate((void *)doc_payload_buffer_, sizeof(docpayload_t) * MAX_DOC_PER_RECORD);
    }
    if (posting_decoder_) {
        posting_decoder_->~MultiPostingDecoder();
        session_pool_->Deallocate((void *)posting_decoder_, sizeof(posting_decoder_));
    }
}

bool PostingIterator::Init(const SharedPtr<Vector<SegmentPosting>> &seg_postings, const u32) {
    segment_postings_ = seg_postings;
    Reset();
    return true;
}

RowID PostingIterator::SeekDoc(RowID row_id) {
    RowID ret = INVALID_ROWID;
    RowID current_row_id = current_row_id_;
    row_id = std::max(current_row_id + 1, row_id);
    if (unlikely(last_doc_id_in_buffer_ == INVALID_ROWID || row_id > last_doc_id_in_buffer_)) {
        if (!posting_decoder_->DecodeDocBuffer(row_id, doc_buffer_, current_row_id, last_doc_id_in_buffer_, current_ttf_))
            return ret;
        doc_buffer_cursor_ = doc_buffer_ + 1;
        posting_option_ = posting_decoder_->GetPostingFormatOption();
    }
    docid_t *cursor = doc_buffer_cursor_;
    while (current_row_id < row_id) {
        current_row_id += *(cursor++);
    }
    current_row_id_ = current_row_id;
    doc_buffer_cursor_ = cursor;
    need_move_to_current_doc_ = true;
    ret = current_row_id;
    return ret;
}

void PostingIterator::MoveToCurrentDoc() {
    need_move_to_current_doc_ = false;
    in_doc_pos_iter_inited_ = false;
    if (posting_option_.HasTermFrequency()) {
        state_.SetRowID(current_row_id_);
        state_.SetTermFreq(InnerGetTF());

        u32 seeked_doc_count = GetCurrentSeekedDocCount();
        state_.SetSeekedDocCount(seeked_doc_count);

        if (posting_option_.HasPositionList()) {
            ttf_t current_ttf = 0;
            current_ttf = GetCurrentTTF();
            posting_decoder_->MoveToCurrentDocPosition(current_ttf);
        }
    }
}

void PostingIterator::SeekPosition(pos_t pos, pos_t &result) {
    if (need_move_to_current_doc_) {
        MoveToCurrentDoc();
    }
    if (!in_doc_pos_iter_inited_) {
        if (posting_option_.HasPositionList()) {
            in_doc_pos_iter_inited_ = true;
            in_doc_pos_iterator_ = posting_decoder_->GetInDocPositionIterator();
            in_doc_pos_iterator_->Init(state_);
        } else {
            result = INVALID_POSITION;
            return;
        }
    }
    in_doc_pos_iterator_->SeekPosition(pos, result);
}

void PostingIterator::Reset() {
    if (!segment_postings_ || segment_postings_->size() == 0) {
        return;
    }
    if (posting_decoder_) {
        posting_decoder_->~MultiPostingDecoder();
        session_pool_->Deallocate((void *)posting_decoder_, sizeof(posting_decoder_));
    }

    posting_decoder_ = new (session_pool_->Allocate(sizeof(MultiPostingDecoder))) MultiPostingDecoder(&state_, session_pool_);
    posting_decoder_->Init(segment_postings_);

    current_row_id_ = INVALID_ROWID;
    last_doc_id_in_buffer_ = INVALID_ROWID;
    need_move_to_current_doc_ = false;
    in_doc_pos_iter_inited_ = false;
}

} // namespace infinity