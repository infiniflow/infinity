#pragma once

#include "pos_list_format_option.h"
#include "storage/invertedindex/index_defines.h"

namespace infinity {

class PositionListDecoder;
class InDocPositionState {
public:
    InDocPositionState(const PositionListFormatOption &option = PositionListFormatOption())
        : pos_decoder_(nullptr), record_offset_(0), offset_in_record_(0), total_position_count_(0) {}
    virtual ~InDocPositionState() = default;

    void SetDocId(docid_t doc_id) { doc_id_ = doc_id; }

    docid_t GetDocId() const { return doc_id_; }

    void SetTermFreq(tf_t tf) { tf_ = tf; }

    tf_t GetTermFreq();

    void SetPositionListDecoder(PositionListDecoder *pos_decoder) { pos_decoder_ = pos_decoder; }

    PositionListDecoder *GetPositionListDecoder() const { return pos_decoder_; }

    void SetSeekedDocCount(uint32_t seeked_doc_count) { seeked_doc_count_ = seeked_doc_count; }

    uint32_t GetSeekedDocCount() const { return seeked_doc_count_; }

    void SetRecordOffset(int32_t record_offset) { record_offset_ = record_offset; }

    int32_t GetRecordOffset() const { return record_offset_; }

    void SetOffsetInRecord(int32_t offset_in_record) { offset_in_record_ = offset_in_record; }

    int32_t GetOffsetInRecord() const { return offset_in_record_; }

    void SetTotalPosCount(uint32_t total_position_count) { total_position_count_ = total_position_count; }

    uint32_t GetTotalPosCount() const { return total_position_count_; }

protected:
    tf_t tf_;
    docid_t doc_id_;
    uint32_t seeked_doc_count_;
    PositionListDecoder *pos_decoder_;
    int32_t record_offset_;
    int32_t offset_in_record_;
    uint32_t total_position_count_;
    PositionListFormatOption option_;
    friend class PositionListDecoder;
    friend class InMemPositionListDecoder;
};

} // namespace infinity