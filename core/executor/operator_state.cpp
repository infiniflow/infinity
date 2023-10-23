//
// Created by JinHai on 2022/9/15.
//

module;

import data_block;

module operator_state;

namespace infinity {

void QueueSourceState::PushData(DataBlock *input_data_block) {
    next_input_state_->input_data_block_ = input_data_block;
    ++next_input_state_->received_data_count_;
    if (next_input_state_->received_data_count_ >= next_input_state_->total_data_count_) {
        complete_ = true;
    }
}

} // namespace infinity