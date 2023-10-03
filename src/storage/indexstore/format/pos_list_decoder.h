#pragma once

#include "common/memory/byte_slice.h"
#include "common/memory/memory_pool.h"
#include "in_doc_pos_state.h"
#include "pair_value_skiplist_reader.h"
#include "pos_list_format_option.h"
#include "position_bitmap_reader.h"
#include "posting_value.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity {

class PositionListDecoder {
public:
    PositionListDecoder(const PositionListFormatOption& option, MemoryPool* sessionPool);
    virtual ~PositionListDecoder();

    void
    Init(ByteSlice* pos_list, tf_t total_tf, uint32_t pos_list_begin, InDocPositionState* state);

    void
    Init(const ByteSliceList* pos_list, tf_t total_tf, uint32_t pos_list_begin, InDocPositionState* state);

    virtual bool
    SkipTo(ttf_t current_ttf, InDocPositionState* state);

    void
    SetNeedReopen(bool need_reopen) { need_reopen_ = need_reopen; }

    // TODO: separate TfBitmap CalculateRecordOffset from LocateRecord
    virtual bool
    LocateRecord(const InDocPositionState* state, uint32_t& tf);

    virtual uint32_t
    DecodeRecord(pos_t* pos_buffer, uint32_t pos_buffer_len);

    uint32_t
    GetRecordOffset() const { return record_offset_; }

    int32_t
    GetOffsetInRecord() const { return offset_in_record_; }

    PositionBitmapReader*
    GetPositionBitmapReader() const { return pos_bitmap_reader_; }

private:
    void
    InitPositionBitmapReader(ByteSlice* pos_list, uint32_t& pos_list_cursor, InDocPositionState* state);

    void
    InitPositionBitmapReader(const ByteSliceList* pos_list, uint32_t& pos_list_cursor, InDocPositionState* state);

    void
    InitPositionBitmapBlockBuffer(ByteSliceReader& pos_list_reader, tf_t total_tf, uint32_t pos_skiplist_start, uint32_t pos_skiplist_len, InDocPositionState* state);

    void
    InitPositionSkipList(const ByteSliceList* pos_list, tf_t total_tf, uint32_t pos_skiplist_start, uint32_t pos_skiplist_len, InDocPositionState* state);

    void
    InitPositionSkipList(ByteSlice* pos_list, tf_t total_tf, uint32_t pos_skiplist_start, uint32_t pos_skiplist_len, InDocPositionState* state);

    void
    CalculateRecordOffsetByPosBitmap(const InDocPositionState* state, uint32_t& tf, uint32_t& record_offset, int32_t& offset_in_record);

protected:
    PairValueSkipListReader* pos_skiplist_reader_;
    MemoryPool* session_pool_;
    const Int32Encoder* pos_encoder_;
    PositionBitmapReader* pos_bitmap_reader_;
    uint32_t* pos_bitmap_block_buffer_;
    uint32_t pos_bitmap_block_count_;
    uint32_t total_tf_;

    uint32_t decoded_pos_count_;
    uint32_t record_offset_;
    uint32_t pre_record_ttf_;
    int32_t offset_in_record_;

    uint32_t pos_list_begin_;
    uint32_t last_decode_offset_;
    PositionListFormatOption option_;
    bool own_pos_bitmap_block_buffer_;
    bool need_reopen_;

    ByteSlice* pos_single_slice_;

private:
    ByteSliceReader pos_list_reader_;
    friend class PositionListDecoderTest;
};

}// namespace infinity
