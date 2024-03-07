module;

export module pos_list_decoder;

import stl;
import byte_slice;
import byte_slice_reader;
import memory_pool;
import pair_value_skiplist_reader;
import pos_list_format_option;
import posting_value;
import index_defines;
import position_bitmap_reader;

namespace infinity {

class InDocPositionState;
export class PositionListDecoder {
public:
    PositionListDecoder(const PositionListFormatOption &option, MemoryPool *sessionPool);
    virtual ~PositionListDecoder();

    void Init(ByteSlice *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state);

    void Init(const ByteSliceList *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state);

    virtual bool SkipTo(ttf_t current_ttf, InDocPositionState *state);

    void SetNeedReopen(bool need_reopen) { need_reopen_ = need_reopen; }

    // TODO: separate TfBitmap CalculateRecordOffset from LocateRecord
    virtual bool LocateRecord(const InDocPositionState *state, u32 &tf);

    virtual u32 DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len);

    u32 GetRecordOffset() const { return record_offset_; }

    i32 GetOffsetInRecord() const { return offset_in_record_; }

    PositionBitmapReader *GetPositionBitmapReader() const { return pos_bitmap_reader_; }

private:
    void InitPositionBitmapReader(ByteSlice *pos_list, u32 &pos_list_cursor, InDocPositionState *state);

    void InitPositionBitmapReader(const ByteSliceList *pos_list, u32 &pos_list_cursor, InDocPositionState *state);

    void InitPositionBitmapBlockBuffer(ByteSliceReader &pos_list_reader,
                                       tf_t total_tf,
                                       u32 pos_skiplist_start,
                                       u32 pos_skiplist_len,
                                       InDocPositionState *state);

    void InitPositionSkipList(const ByteSliceList *pos_list, tf_t total_tf, u32 pos_skiplist_start, u32 pos_skiplist_len, InDocPositionState *state);

    void InitPositionSkipList(ByteSlice *pos_list, tf_t total_tf, u32 pos_skiplist_start, u32 pos_skiplist_len, InDocPositionState *state);

    void CalculateRecordOffsetByPosBitmap(const InDocPositionState *state, u32 &tf, u32 &record_offset, i32 &offset_in_record);

protected:
    PairValueSkipListReader *pos_skiplist_reader_;
    MemoryPool *session_pool_;
    const Int32Encoder *pos_encoder_;
    PositionBitmapReader *pos_bitmap_reader_;
    u32 *pos_bitmap_block_buffer_;
    u32 pos_bitmap_block_count_;
    u32 total_tf_;

    u32 decoded_pos_count_;
    u32 record_offset_;
    u32 pre_record_ttf_;
    i32 offset_in_record_;

    u32 pos_list_begin_;
    u32 last_decode_offset_;
    PositionListFormatOption option_;
    bool own_pos_bitmap_block_buffer_;
    bool need_reopen_;

    ByteSlice *pos_single_slice_;

private:
    ByteSliceReader pos_list_reader_;
    friend class PositionListDecoderTest;
};

} // namespace infinity
