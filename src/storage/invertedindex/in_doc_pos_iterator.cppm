module;

import stl;
import in_doc_pos_state;
import pos_list_format_option;
import index_defines;
export module in_doc_pos_iterator;

namespace infinity {
export class InDocPositionIterator {
public:
    InDocPositionIterator(PositionListFormatOption option = PositionListFormatOption());

    ~InDocPositionIterator() {}

    void Init(const InDocPositionState &state);

    void SeekPosition(pos_t pos, pos_t &result);

private:
    pos_t pos_buffer_[MAX_POS_PER_RECORD];
    InDocPositionState state_;
    i64 current_pos_;
    i32 visited_pos_in_buffer_;
    i32 visited_pos_in_doc_;
    i32 pos_count_in_buffer_;
    u32 offset_in_record_;
    u32 total_pos_count_;
    u32 current_field_id_;
    PositionListFormatOption option_;
};

} // namespace infinity