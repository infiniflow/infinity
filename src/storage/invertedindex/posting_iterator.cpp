module;

#include "common/utility/builtin.h"
#include <cassert>
#include <vector>
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

PostingIterator::PostingIterator(optionflag_t flag, MemoryPool *session_pool) : posting_option_(flag), session_pool_(session_pool) {
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

bool PostingIterator::Init(SharedPtr<Vector<SegmentPosting>> seg_postings, const u32) {
    segment_postings_ = std::move(seg_postings);
    for (auto &seg_posting : *segment_postings_) {
        doc_freq_ += seg_posting.GetTermMeta().GetDocFreq();
    }
    Reset();
    return true;
}

bool PostingIterator::SkipTo(RowID doc_id) {
    if (doc_id > last_doc_id_in_current_block_ or last_doc_id_in_current_block_ == INVALID_ROWID) {
        finish_decode_docid_ = false;
        return posting_decoder_->SkipTo(doc_id,
                                        last_doc_id_in_prev_block_,
                                        lowest_possible_doc_id_in_current_block_,
                                        last_doc_id_in_current_block_,
                                        current_ttf_);
    }
    return true;
}

// u32: block max tf
// u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
Pair<u32, u16> PostingIterator::GetBlockMaxInfo() const { return posting_decoder_->GetBlockMaxInfo(); }

RowID PostingIterator::SeekDoc(RowID row_id) {
    RowID current_row_id = finish_decode_docid_ ? current_row_id_ : INVALID_ROWID;
    if (row_id == current_row_id) [[unlikely]] {
        return current_row_id;
    }
    if (current_row_id != INVALID_ROWID and row_id < current_row_id) {
        return current_row_id;
    }
    assert(row_id > current_row_id or current_row_id == INVALID_ROWID);
    if (!SkipTo(row_id)) {
        current_row_id_ = INVALID_ROWID;
        return INVALID_ROWID;
    }
    if (!finish_decode_docid_) {
        posting_decoder_->DecodeCurrentDocIDBuffer(doc_buffer_);
        current_row_id = last_doc_id_in_prev_block_ + doc_buffer_[0];
        doc_buffer_cursor_ = doc_buffer_ + 1;
        finish_decode_docid_ = true;
    }
    docid_t *cursor = doc_buffer_cursor_;
    while (current_row_id < row_id) {
        current_row_id += *(cursor++);
    }
    current_row_id_ = current_row_id;
    doc_buffer_cursor_ = cursor;
    need_move_to_current_doc_ = true;
    return current_row_id;
}

Pair<bool, RowID> PostingIterator::PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond) {
    if (!finish_decode_docid_) {
        posting_decoder_->DecodeCurrentDocIDBuffer(doc_buffer_);
        current_row_id_ = last_doc_id_in_prev_block_ + doc_buffer_[0];
        doc_buffer_cursor_ = doc_buffer_ + 1;
        finish_decode_docid_ = true;
    }
    RowID current_row_id = current_row_id_;
    docid_t *cursor = doc_buffer_cursor_;
    while (current_row_id < doc_id) {
        current_row_id += *(cursor++);
    }
    if (current_row_id <= doc_id_no_beyond) {
        return {true, current_row_id};
    }
    return {false, INVALID_ROWID};
}

void PostingIterator::MoveToCurrentDoc(bool fetch_position) {
    need_move_to_current_doc_ = false;
    in_doc_pos_iter_inited_ = false;
    if (posting_option_.HasTermFrequency()) {
        state_.SetRowID(current_row_id_);
        state_.SetTermFreq(GetCurrentTF());

        u32 seeked_doc_count = GetCurrentSeekedDocCount();
        state_.SetSeekedDocCount(seeked_doc_count);

        if (fetch_position && posting_option_.HasPositionList()) {
            ttf_t current_ttf = 0;
            current_ttf = GetCurrentTTF();
            posting_decoder_->MoveToCurrentDocPosition(current_ttf);
        }
    }
}

void PostingIterator::SeekPosition(pos_t pos, pos_t &result) {
    if (need_move_to_current_doc_) {
        MoveToCurrentDoc(true);
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

    posting_decoder_ = new (session_pool_->Allocate(sizeof(MultiPostingDecoder))) MultiPostingDecoder(posting_option_, &state_, session_pool_);
    posting_decoder_->Init(segment_postings_);

    current_row_id_ = INVALID_ROWID;
    last_doc_id_in_prev_block_ = INVALID_ROWID;
    last_doc_id_in_current_block_ = INVALID_ROWID;
    current_ttf_ = 0;
    need_move_to_current_doc_ = false;
    in_doc_pos_iter_inited_ = false;
}

} // namespace infinity