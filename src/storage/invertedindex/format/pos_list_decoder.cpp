module;

#include "common/utility/builtin.h"

import stl;
import byte_slice;
import byte_slice_reader;
import memory_pool;
import in_doc_pos_state;
import pair_value_skiplist_reader;
import pos_list_format_option;
import posting_value;
import short_list_optimize_util;
import index_defines;
import position_bitmap_reader;
import index_defines;
module pos_list_decoder;

namespace infinity {

PositionListDecoder::PositionListDecoder(const PositionListFormatOption &option, MemoryPool *session_pool)
    : pos_skiplist_reader_(nullptr), session_pool_(session_pool), pos_encoder_(nullptr), pos_bitmap_reader_(nullptr),
      pos_bitmap_block_buffer_(nullptr), pos_bitmap_block_count_(0), total_tf_(0), decoded_pos_count_(0), record_offset_(0), pre_record_ttf_(0),
      offset_in_record_(0), pos_list_begin_(0), last_decode_offset_(0), option_(option), own_pos_bitmap_block_buffer_(false), need_reopen_(true),
      pos_single_slice_(nullptr) {}

PositionListDecoder::~PositionListDecoder() {
    if (session_pool_) {
        if (pos_skiplist_reader_) {
            pos_skiplist_reader_->~PairValueSkipListReader();
            session_pool_->Deallocate((void *)pos_skiplist_reader_, sizeof(PairValueSkipListReader));
        }
        if (pos_bitmap_reader_) {
            pos_bitmap_reader_->~PositionBitmapReader();
            session_pool_->Deallocate((void *)pos_bitmap_reader_, sizeof(PositionBitmapReader));
        }
    } else {
        if (pos_skiplist_reader_)
            delete pos_skiplist_reader_;
        if (pos_bitmap_reader_)
            delete pos_bitmap_reader_;
        pos_skiplist_reader_ = nullptr;
        pos_bitmap_reader_ = nullptr;
    }
    if (own_pos_bitmap_block_buffer_) {
        if (session_pool_) {
            session_pool_->Deallocate((void *)pos_bitmap_block_buffer_, sizeof(u32) * pos_bitmap_block_count_);
        } else {
            delete[] pos_bitmap_block_buffer_;
        }
        pos_bitmap_block_buffer_ = nullptr;
    }
}

void PositionListDecoder::InitPositionBitmapReader(const ByteSliceList *pos_list, u32 &pos_list_cursor, InDocPositionState *state) {
    u32 bitmapListBegin = pos_list_cursor;
    pos_bitmap_reader_ =
        session_pool_ ? new ((session_pool_)->Allocate(sizeof(PositionBitmapReader))) PositionBitmapReader() : new PositionBitmapReader;
    pos_bitmap_reader_->Init(pos_list, bitmapListBegin);
    pos_list_cursor += pos_bitmap_reader_->GetBitmapListSize();
}

void PositionListDecoder::InitPositionBitmapReader(ByteSlice *pos_list, u32 &pos_list_cursor, InDocPositionState *state) {
    u32 bitmapListBegin = pos_list_cursor;
    pos_bitmap_reader_ =
        session_pool_ ? new ((session_pool_)->Allocate(sizeof(PositionBitmapReader))) PositionBitmapReader() : new PositionBitmapReader;
    pos_bitmap_reader_->Init(pos_list, bitmapListBegin);
    pos_list_cursor += pos_bitmap_reader_->GetBitmapListSize();
}

void PositionListDecoder::InitPositionBitmapBlockBuffer(ByteSliceReader &pos_list_reader,
                                                        tf_t total_tf,
                                                        u32 pos_skiplist_start,
                                                        u32 pos_skiplist_len,
                                                        InDocPositionState *state) {
    pos_bitmap_block_count_ = pos_skiplist_len / sizeof(u32);
    pos_bitmap_block_buffer_ =
        session_pool_ ? (u32 *)session_pool_->Allocate(pos_bitmap_block_count_ * sizeof(u32)) : new u32[pos_bitmap_block_count_];

    void *tmpPtr = pos_bitmap_block_buffer_;
    pos_list_reader.ReadMayCopy(tmpPtr, pos_skiplist_len);
    if (tmpPtr == pos_bitmap_block_buffer_) {
        own_pos_bitmap_block_buffer_ = true;
    } else {
        own_pos_bitmap_block_buffer_ = false;
        if (session_pool_) {
            session_pool_->Deallocate((void *)pos_bitmap_block_buffer_, sizeof(u32) * pos_bitmap_block_count_);
        } else {
            delete[] pos_bitmap_block_buffer_;
        }
        pos_bitmap_block_buffer_ = (u32 *)tmpPtr;
    }
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
        pos_skiplist_reader_ =
            session_pool_ ? new ((session_pool_)->Allocate(sizeof(PairValueSkipListReader))) PairValueSkipListReader : new PairValueSkipListReader;
        pos_skiplist_reader_->Load(pos_list, pos_skiplist_start, pos_skiplist_end, (total_tf - 1) / MAX_POS_PER_RECORD + 1);
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
        pos_skiplist_reader_ =
            session_pool_ ? new ((session_pool_)->Allocate(sizeof(PairValueSkipListReader))) PairValueSkipListReader() : new PairValueSkipListReader;
        pos_skiplist_reader_->Load(pos_list, pos_skiplist_start, pos_skiplist_end, (total_tf - 1) / MAX_POS_PER_RECORD + 1);
        decoded_pos_count_ = 0;
    }
}

void PositionListDecoder::Init(ByteSlice *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state) {
    pos_single_slice_ = pos_list;

    total_tf_ = total_tf;
    pos_encoder_ = GetPosListEncoder();

    if (option_.HasTfBitmap()) {
        InitPositionBitmapReader(pos_list, pos_list_begin, state);
    }

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

    if (option_.HasTfBitmap()) {
        InitPositionBitmapBlockBuffer(pos_list_reader_, total_tf, pos_skiplist_start, pos_skiplist_len, state);
    } else {
        InitPositionSkipList(pos_list, total_tf, pos_skiplist_start, pos_skiplist_len, state);
    }

    pos_list_reader_.Seek(pos_list_begin_);
}

void PositionListDecoder::Init(const ByteSliceList *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state) {
    total_tf_ = total_tf;
    pos_encoder_ = GetPosListEncoder();
    if (option_.HasTfBitmap()) {
        InitPositionBitmapReader(pos_list, pos_list_begin, state);
    }

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

    if (option_.HasTfBitmap()) {
        InitPositionBitmapBlockBuffer(pos_list_reader_, total_tf, pos_skiplist_start, pos_skiplist_len, state);
    } else {
        InitPositionSkipList(pos_list, total_tf, pos_skiplist_start, pos_skiplist_len, state);
    }

    pos_list_reader_.Seek(pos_list_begin_);
}

bool PositionListDecoder::SkipTo(ttf_t current_ttf, InDocPositionState *state) {
    if (!option_.HasPositionList() || option_.HasTfBitmap()) {
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

    if (option_.HasTfBitmap()) {
        CalculateRecordOffsetByPosBitmap(state, tf, record_offset, offset_in_record);
    } else {
        record_offset = state->GetRecordOffset();
        offset_in_record = state->GetOffsetInRecord();
        tf = state->tf_;
    }
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

void PositionListDecoder::CalculateRecordOffsetByPosBitmap(const InDocPositionState *state, u32 &tf, u32 &record_offset, i32 &offset_in_record) {
    PositionCountInfo info = pos_bitmap_reader_->GetPosCountInfo(state->GetSeekedDocCount());
    tf = info.current_doc_pos_count_;
    u32 pre_agg_pos_count = info.pre_doc_agg_pos_count_;
    record_offset = record_offset_;
    u32 decode_block_pos_count = (decoded_pos_count_ & MAX_POS_PER_RECORD_MASK);
    if (likely(decode_block_pos_count == 0)) {
        decode_block_pos_count = MAX_POS_PER_RECORD;
    }

    if (pre_agg_pos_count >= decoded_pos_count_ || pre_agg_pos_count < decoded_pos_count_ - decode_block_pos_count) {
        u32 blockIndex = pre_agg_pos_count >> MAX_POS_PER_RECORD_BIT_NUM;
        u32 offset = blockIndex == 0 ? 0 : pos_bitmap_block_buffer_[blockIndex - 1];
        decoded_pos_count_ = blockIndex == pos_bitmap_block_count_ ? total_tf_ : (blockIndex + 1) << MAX_POS_PER_RECORD_BIT_NUM;
        decode_block_pos_count = (decoded_pos_count_ & MAX_POS_PER_RECORD_MASK);
        if (likely(decode_block_pos_count == 0)) {
            decode_block_pos_count = MAX_POS_PER_RECORD;
        }
        record_offset = pos_list_begin_ + offset;
    }
    offset_in_record = pre_agg_pos_count - (decoded_pos_count_ - decode_block_pos_count);
}

u32 PositionListDecoder::DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len) {
    last_decode_offset_ = pos_list_reader_.Tell();
    auto pos_count = pos_encoder_->Decode(pos_buffer, pos_buffer_len, pos_list_reader_);
    return pos_count;
}
} // namespace infinity
