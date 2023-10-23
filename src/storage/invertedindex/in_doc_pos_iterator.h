#pragma once

#include "format/in_doc_pos_state.h"
#include "format/pos_list_format_option.h"

namespace infinity {
class InDocPositionIterator {
public:
    InDocPositionIterator(PositionListFormatOption option = PositionListFormatOption());

    ~InDocPositionIterator() {}

    void Init(const InDocPositionState &state);

    void SeekPosition(pos_t pos, pos_t &result);

private:
    pos_t pos_buffer_[MAX_POS_PER_RECORD];
    InDocPositionState state_;
    int64_t current_pos_;
    int32_t visited_pos_in_buffer_;
    int32_t visited_pos_in_doc_;
    int32_t pos_count_in_buffer_;
    uint32_t offset_in_record_;
    uint32_t total_pos_count_;
    uint32_t current_field_id_;
    PositionListFormatOption option_;
};

} // namespace infinity