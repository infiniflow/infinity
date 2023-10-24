#include "multi_posting_decoder.h"
#include "format/pair_value_skiplist_reader.h"
#include "format/tri_value_skiplist_reader.h"

namespace infinity {

MultiPostingDecoder::MultiPostingDecoder(InDocPositionState *state, MemoryPool *pool)
    : base_doc_id_(0), need_decode_tf_(false), need_decode_doc_payload_(false), skiplist_decoder_(nullptr), segment_cursor_(0), segment_count_(0),
      session_pool_(pool), in_doc_pos_iterator_(nullptr), in_doc_state_keeper_(state, pool) {}

MultiPostingDecoder::~MultiPostingDecoder() {
    if (skiplist_decoder_) {
        if (session_pool_) {
            skiplist_decoder_->~SkipIndexDecoder();
            session_pool_->Deallocate((void *)skiplist_decoder_, sizeof(skiplist_decoder_));
        } else
            delete skiplist_decoder_;
    }
}

bool MultiPostingDecoder::MoveToSegment(docid_t start_doc_id) { return true; }

bool MultiPostingDecoder::DecodeDocBufferInOneSegment(docid_t start_doc_id,
                                                      docid_t *doc_buffer,
                                                      docid_t &first_doc_id,
                                                      docid_t &last_doc_id,
                                                      ttf_t &current_ttf) {
    docid_t next_seg_base_doc_id = GetSegmentBaseDocId(segment_cursor_);
    if (next_seg_base_doc_id != INVALID_DOCID && start_doc_id >= next_seg_base_doc_id) {
        // start docid not in current segment
        return false;
    }

    docid_t cur_seg_doc_id = std::max(docid_t(0), start_doc_id - base_doc_id_);
    if (!skiplist_decoder_->DecodeDocBuffer(cur_seg_doc_id, doc_buffer, first_doc_id, last_doc_id, current_ttf)) {
        return false;
    }
    need_decode_tf_ = cur_segment_format_option_.HasTfList();
    need_decode_doc_payload_ = cur_segment_format_option_.HasDocPayload();

    first_doc_id += base_doc_id_;
    last_doc_id += base_doc_id_;
    return true;
}

SkipIndexDecoder *MultiPostingDecoder::CreateSkiplistDecoder(uint32_t doc_list_begin_pos, bool enableShortListVbyteCompress) {
    if (cur_segment_format_option_.HasTfList()) {
        skiplist_decoder_ = session_pool_ ? (new ((session_pool_)->Allocate(sizeof(SkipListDecoder<TriValueSkipListReader>)))
                                                 SkipListDecoder<TriValueSkipListReader>(session_pool_, &doc_list_reader_, doc_list_begin_pos))
                                          : new SkipListDecoder<TriValueSkipListReader>(session_pool_, &doc_list_reader_, doc_list_begin_pos);
    } else {
        skiplist_decoder_ = session_pool_ ? (new ((session_pool_)->Allocate(sizeof(SkipListDecoder<TriValueSkipListReader>)))
                                                 SkipListDecoder<PairValueSkipListReader>(session_pool_, &doc_list_reader_, doc_list_begin_pos))
                                          : new SkipListDecoder<PairValueSkipListReader>(session_pool_, &doc_list_reader_, doc_list_begin_pos);
    }
}
} // namespace infinity