module;

export module position_list_decoder;

import stl;
import byte_slice;
import byte_slice_reader;
import memory_pool;
import skiplist_reader;
import posting_list_format;
import position_list_format_option;
import posting_field;
import index_defines;

namespace infinity {

class InDocPositionState;
export class PositionListDecoder {
public:
    PositionListDecoder(const PostingFormatOption &option, MemoryPool *sessionPool);
    virtual ~PositionListDecoder();

    void Init(ByteSlice *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state);

    void Init(const ByteSliceList *pos_list, tf_t total_tf, u32 pos_list_begin, InDocPositionState *state);

    virtual bool SkipTo(ttf_t current_ttf, InDocPositionState *state);

    void SetNeedReopen(bool need_reopen) { need_reopen_ = need_reopen; }

    virtual bool LocateRecord(const InDocPositionState *state, u32 &tf);

    virtual u32 DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len);

    u32 GetRecordOffset() const { return record_offset_; }

    i32 GetOffsetInRecord() const { return offset_in_record_; }

private:
    void InitPositionSkipList(const ByteSliceList *pos_list, tf_t total_tf, u32 pos_skiplist_start, u32 pos_skiplist_len, InDocPositionState *state);

    void InitPositionSkipList(ByteSlice *pos_list, tf_t total_tf, u32 pos_skiplist_start, u32 pos_skiplist_len, InDocPositionState *state);

protected:
    SkipListReader *pos_skiplist_reader_;
    u32 skiplist_reader_real_size_ = 0;
    MemoryPool *session_pool_;
    const Int32Encoder *pos_encoder_;
    u32 total_tf_;

    u32 decoded_pos_count_;
    u32 record_offset_;
    u32 pre_record_ttf_;
    i32 offset_in_record_;

    u32 pos_list_begin_;
    u32 last_decode_offset_;
    PostingFormatOption option_;
    bool need_reopen_;

    ByteSlice *pos_single_slice_;

private:
    ByteSliceReader pos_list_reader_;
    friend class PositionListDecoderTest;
};

} // namespace infinity
