module;

module in_doc_pos_iterator;

import stl;
import index_defines;
import in_doc_pos_state;
import position_list_format_option;
import position_list_decoder;

namespace infinity {
InDocPositionIterator::InDocPositionIterator(const PositionListFormatOption &option)
    : current_pos_(-1), visited_pos_in_buffer_(-1), visited_pos_in_doc_(-1), pos_count_in_buffer_(0), offset_in_record_(0), total_pos_count_(0),
      current_field_id_(-1), option_(option), state_(option) {}

void InDocPositionIterator::Init(const InDocPositionState &state) {
    PositionListDecoder *pos_decoder = state.GetPositionListDecoder();

    if (state_.GetPositionListDecoder() != pos_decoder) {
        pos_decoder->SetNeedReopen(true);
    }

    bool need_unpack = pos_decoder->LocateRecord(&state, total_pos_count_);
    offset_in_record_ = pos_decoder->GetOffsetInRecord();

    state_ = state;

    if (need_unpack) {
        visited_pos_in_buffer_ = -1;
        pos_count_in_buffer_ = 0;
    } else {
        visited_pos_in_buffer_ = offset_in_record_ - 1;
        pos_buffer_[visited_pos_in_buffer_ + 1] += 1;
    }

    current_pos_ = -1;
    visited_pos_in_doc_ = 0;
    current_field_id_ = -1;
}

void InDocPositionIterator::SeekPosition(pos_t pos, pos_t &result) {
    pos = pos >= (current_pos_ + 1) ? pos : (current_pos_ + 1);
    while (pos > current_pos_) {
        if (++visited_pos_in_doc_ > (i32)total_pos_count_) {
            result = INVALID_POSITION;
            return;
        }

        if (++visited_pos_in_buffer_ >= pos_count_in_buffer_) {
            PositionListDecoder *pos_decoder = state_.GetPositionListDecoder();
            pos_count_in_buffer_ = pos_decoder->DecodeRecord(pos_buffer_, MAX_POS_PER_RECORD);

            if (visited_pos_in_buffer_ == 0) {
                visited_pos_in_buffer_ = offset_in_record_;
                pos_buffer_[visited_pos_in_buffer_] += 1;
            } else {
                visited_pos_in_buffer_ = 0;
            }
        }
        current_pos_ += pos_buffer_[visited_pos_in_buffer_];
    }
    result = current_pos_;
}

} // namespace infinity