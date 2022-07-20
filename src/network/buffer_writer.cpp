//
// Created by JinHai on 2022/7/20.
//

#include "buffer_writer.h"

namespace infinity {

BufferWriter::BufferWriter(const hv::SocketChannelPtr &channel) : channel_(channel) {
}

void
BufferWriter::flush() {
    Assert(current_pos_ + 1 <= INT_MAX, "Flush data size is weird, panic!");
    int data_size = static_cast<int>(current_pos_ + 1);
    auto sent_size = channel_->write(data_, data_size);
    if(sent_size == -1) {
        // Nothing can be done except panic.
        Assert(false, "Send data error, panic!");
    }
    if(sent_size != current_pos_ + 1) {
        // TODO: error message should include the sent size.
        Assert(false, "Send data size isn't correct, panic!");
    }
    current_pos_ = 0;
}

void
BufferWriter::send_string(const std::string& value, NullTerminator null_terminator) {
    // Data after the position which will be sent.
    uint64_t value_pos = 0;

    while(value_pos < value.size()) {
        const uint64_t sending_size = std::min(value.size() - value_pos, available_capacity());
        memcpy(data_, value.c_str(), sending_size);
        value_pos += sending_size;
        current_pos_ += sending_size;
        if(available_capacity() == 0) flush();
    }

    if(null_terminator == NullTerminator::kYes) {
        while(1 + current_pos_ >= WRITE_BUFFER_SIZE) {
            flush();
        }
        data_[current_pos_] = '\0';
        ++ current_pos_;
    }
}

}
