module;
#include <cassert>

import stl;
import memory_pool;
import pos_list_decoder;
import buffered_byte_slice_reader;
import in_doc_pos_state;
import short_list_optimize_util;
import pos_list_format_option;
import pair_value_skiplist_reader;
import index_defines;
module inmem_pos_list_decoder;

namespace infinity {

InMemPositionListDecoder::InMemPositionListDecoder(const PositionListFormatOption &option, MemoryPool *session_pool)
    : PositionListDecoder(option, session_pool), pos_list_buffer_(nullptr) {}

InMemPositionListDecoder::~InMemPositionListDecoder() {
    if (session_pool_) {
        pos_list_buffer_->~BufferedByteSlice();
        session_pool_->Deallocate((void *)pos_list_buffer_, sizeof(BufferedByteSlice));
    } else {
        delete pos_list_buffer_;
        pos_list_buffer_ = nullptr;
    }
}

void InMemPositionListDecoder::Init(ttf_t total_tf, PairValueSkipListReader *skip_list_reader, BufferedByteSlice *pos_list_buffer) {
    total_tf_ = total_tf;
    pos_skiplist_reader_ = skip_list_reader;
    pos_list_buffer_ = pos_list_buffer;
    pos_list_reader_.Open(pos_list_buffer);
    decoded_pos_count_ = 0;
}

bool InMemPositionListDecoder::SkipTo(ttf_t current_ttf, InDocPositionState *state) {
    if (total_tf_ <= current_ttf) {
        return false;
    }

    if (!pos_skiplist_reader_) {
        assert(total_tf_ <= MAX_POS_PER_RECORD);
        state->SetRecordOffset(0);
        state->SetOffsetInRecord(current_ttf);
        return true;
    }

    if ((i32)decoded_pos_count_ > current_ttf) {
        // skipTo only used with mState in in_doc_state_keeper
        // in same record, mState no need reset RecordOffset
        state->SetOffsetInRecord(current_ttf - pre_record_ttf_);
        return true;
    }

    u32 offset, len;
    auto ret = pos_skiplist_reader_->SkipTo(current_ttf + 1, decoded_pos_count_, offset, len);
    if (ret) {
        pre_record_ttf_ = pos_skiplist_reader_->GetPrevKey();
    } else {
        offset = pos_skiplist_reader_->GetLastValueInBuffer();
        pre_record_ttf_ = pos_skiplist_reader_->GetCurrentKey();
        decoded_pos_count_ = total_tf_;
    }

    state->SetRecordOffset(offset);
    state->SetOffsetInRecord(current_ttf - pre_record_ttf_);
    return true;
}

bool InMemPositionListDecoder::LocateRecord(const InDocPositionState *state, u32 &tf) {
    assert(!option_.HasTfBitmap());
    record_offset_ = state->GetRecordOffset();
    offset_in_record_ = state->GetOffsetInRecord();
    tf = state->tf_;

    if (!need_reopen_ && last_decode_offset_ == record_offset_) {
        // no need to relocate
        return false;
    }

    if (need_reopen_ || last_decode_offset_ > record_offset_) {
        pos_list_reader_.Open(pos_list_buffer_);
        last_decode_offset_ = 0;
    }
    pos_list_reader_.Seek(record_offset_);
    need_reopen_ = false;
    return true;
}

u32 InMemPositionListDecoder::DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len) {
    last_decode_offset_ = pos_list_reader_.Tell();

    SizeT pos_count = 0;
    pos_list_reader_.Decode(pos_buffer, pos_buffer_len, pos_count);

    return pos_count;
}
} // namespace infinity
