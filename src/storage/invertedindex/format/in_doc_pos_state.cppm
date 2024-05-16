module;

export module in_doc_pos_state;

import stl;
import position_list_format_option;
import index_defines;
import internal_types;

namespace infinity {

class PositionListDecoder;
export class InDocPositionState {
public:
    InDocPositionState(const PositionListFormatOption &option)
        : pos_decoder_(nullptr), record_offset_(0), offset_in_record_(0), total_position_count_(0), option_(option) {}
    virtual ~InDocPositionState() = default;

    void SetRowID(RowID row_id) { row_id_ = row_id; }

    RowID GetRowID() const { return row_id_; }

    void SetTermFreq(tf_t tf) { tf_ = tf; }

    tf_t GetTermFreq();

    void SetPositionListDecoder(PositionListDecoder *pos_decoder);

    PositionListDecoder *GetPositionListDecoder() const { return pos_decoder_; }

    void SetSeekedDocCount(u32 seeked_doc_count) { seeked_doc_count_ = seeked_doc_count; }

    u32 GetSeekedDocCount() const { return seeked_doc_count_; }

    void SetRecordOffset(i32 record_offset) { record_offset_ = record_offset; }

    i32 GetRecordOffset() const { return record_offset_; }

    void SetOffsetInRecord(i32 offset_in_record) { offset_in_record_ = offset_in_record; }

    i32 GetOffsetInRecord() const { return offset_in_record_; }

    void SetTotalPosCount(u32 total_position_count) { total_position_count_ = total_position_count; }

    u32 GetTotalPosCount() const { return total_position_count_; }

public:
    tf_t tf_;
    RowID row_id_;
    u32 seeked_doc_count_;
    PositionListDecoder *pos_decoder_{nullptr};
    i32 record_offset_;
    i32 offset_in_record_;
    u32 total_position_count_;
    PositionListFormatOption option_;
};

} // namespace infinity