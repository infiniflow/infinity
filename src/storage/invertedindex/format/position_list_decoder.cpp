module;

#include "common/utility/builtin.h"

import stl;
import byte_slice;
import byte_slice_reader;

import in_doc_pos_state;
import skiplist_reader;
import position_list_format_option;
import posting_field;
import short_list_optimize_util;
import index_defines;
import third_party;

module position_list_decoder;

namespace infinity {

PositionListDecoder::PositionListDecoder(const PostingFormatOption &option)
    : pos_skiplist_reader_(nullptr), pos_encoder_(nullptr), total_tf_(0), decoded_pos_count_(0), record_offset_(0), pre_record_ttf_(0),
      offset_in_record_(0), pos_list_begin_(0), last_decode_offset_(0), option_(option), need_reopen_(true), pos_single_slice_(nullptr) {}

PositionListDecoder::~PositionListDecoder() {
    if (pos_skiplist_reader_)
        delete pos_skiplist_reader_;
    pos_skiplist_reader_ = nullptr;
}

void PositionListDecoder::InitPositionSkipList(const ByteSliceList *pos_list,
                                               tf_t total_tf,
                                               u32 pos_skiplist_start,
                                               u32 pos_skiplist_len,
                                               InDocPositionState *state) {
    u32 pos_skiplist_end = pos_skiplist_start + pos_skiplist_len;
    if (ShortListOptimizeUtil::IsShortPosList(total_tf)) {
        decoded_pos_count_ = total_tf;
        state->SetRecordOffset(pos_skiplist_end);
    } else {
        pos_skiplist_reader_ = new SkipListReaderByteSlice(option_.GetPosListFormatOption());
        skiplist_reader_real_size_ = sizeof(SkipListReaderByteSlice);
        static_cast<SkipListReaderByteSlice *>(pos_skiplist_reader_)->Load(pos_list, pos_skiplist_start, pos_skiplist_end);
        decoded_pos_count_ = 0;
    }
}

void PositionListDecoder::InitPositionSkipList(ByteSlice *pos_list,
                                               tf_t total_tf,
                                               u32 pos_skiplist_start,
                                               u32 pos_skiplist_len,
                                               InDocPositionState *state) {
    u32 pos_skiplist_end = pos_skiplist_start + pos_skiplist_len;
    if (ShortListOptimizeUtil::IsShortPosList(total_tf)) {
        decoded_pos_count_ = total_tf;
        state->SetRecordOffset(pos_skiplist_end);
    } else {
        pos_skiplist_reader_ = new SkipListReaderByteSlice(option_.GetPosListFormatOption());
        skiplist_reader_real_size_ = sizeof(SkipListReaderByteSlice);
        static_cast<SkipListReaderByteSlice *>(pos_skiplist_reader_)->Load(pos_list, pos_skiplist_start, pos_skiplist_end);
        decoded_pos_count_ = 0;
    }
}

void PositionListDecoder::Init(ByteSlice *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state) {
    pos_single_slice_ = pos_list;

    total_tf_ = total_tf;
    pos_encoder_ = GetPosListEncoder();

    if (!option_.HasPositionList()) {
        return;
    }

    pos_list_reader_.Open(pos_list);
    pos_list_reader_.Seek(pos_list_begin);
    u32 pos_skiplist_len = pos_list_reader_.ReadVUInt32();
    // read pos list length
    pos_list_reader_.ReadVUInt32();

    u32 pos_skiplist_start = pos_list_reader_.Tell();
    pos_list_begin_ = pos_skiplist_start + pos_skiplist_len;

    InitPositionSkipList(pos_list, total_tf, pos_skiplist_start, pos_skiplist_len, state);

    pos_list_reader_.Seek(pos_list_begin_);
}

void PositionListDecoder::Init(const ByteSliceList *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state) {
    total_tf_ = total_tf;
    pos_encoder_ = GetPosListEncoder();

    if (!option_.HasPositionList()) {
        return;
    }

    pos_list_reader_.Open(const_cast<ByteSliceList *>(pos_list));
    pos_list_reader_.Seek(pos_list_begin);
    u32 pos_skiplist_len = pos_list_reader_.ReadVUInt32();
    // read pos list length
    pos_list_reader_.ReadVUInt32();

    u32 pos_skiplist_start = pos_list_reader_.Tell();
    pos_list_begin_ = pos_skiplist_start + pos_skiplist_len;

    InitPositionSkipList(pos_list, total_tf, pos_skiplist_start, pos_skiplist_len, state);

    pos_list_reader_.Seek(pos_list_begin_);
}

bool PositionListDecoder::SkipTo(ttf_t current_ttf, InDocPositionState *state) {
    if (!option_.HasPositionList()) {
        return true;
    }

    if ((i32)decoded_pos_count_ > current_ttf) {
        state->SetOffsetInRecord(current_ttf - pre_record_ttf_);
        return true;
    }

    u32 offset, len;
    if (pos_skiplist_reader_ == nullptr) {
        return false;
    }
    auto ret = pos_skiplist_reader_->SkipTo(current_ttf + 1, decoded_pos_count_, offset, len);
    if (!ret) {
        return false;
    }
    state->SetRecordOffset(offset + pos_list_begin_);
    pre_record_ttf_ = pos_skiplist_reader_->GetPrevKey();
    state->SetOffsetInRecord(current_ttf - pre_record_ttf_);
    return true;
}

bool PositionListDecoder::LocateRecord(const InDocPositionState *state, u32 &tf) {
    u32 record_offset;
    i32 offset_in_record;

    record_offset = state->GetRecordOffset();
    offset_in_record = state->GetOffsetInRecord();
    tf = state->tf_;

    record_offset_ = record_offset;
    offset_in_record_ = offset_in_record;

    ByteSliceList *position_list = pos_list_reader_.GetByteSliceList();
    if (!need_reopen_ && last_decode_offset_ == record_offset_) {
        // no need to relocate
        return false;
    }

    if (need_reopen_ || last_decode_offset_ > record_offset_) {
        if (pos_single_slice_) {
            pos_list_reader_.Open(pos_single_slice_);
        } else {
            pos_list_reader_.Open(position_list);
        }
        last_decode_offset_ = 0;
    }
    pos_list_reader_.Seek(record_offset_);
    need_reopen_ = false;
    return true;
}

u32 PositionListDecoder::DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len) {
    last_decode_offset_ = pos_list_reader_.Tell();
    auto pos_count = pos_encoder_->Decode(pos_buffer, pos_buffer_len, pos_list_reader_);
    return pos_count;
}

} // namespace infinity
