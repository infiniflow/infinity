//
// Created by JinHai on 2022/7/20.
//

#include "buffer_reader.h"

namespace infinity {

BufferReader::BufferReader(const hv::SocketChannelPtr &channel) : channel_(channel){

}

std::string
BufferReader::read_by_size(uint64_t read_size, NullTerminator is_null) {
    std::string message;
//    if(unread_buffer_size() > 0) {
//        message.append((char*)(buffer_->data()) + start_pos_, std::min(unread_buffer_size(), read_size));
//        start_pos_ += message.size();
//    }

    while(message.size() < read_size) {
        if(unread_buffer_size() == 0) {
            // TODO: Read more data
            Assert(false, "Need to read more data, here");
        }
        auto insert_size = std::min(read_size - message.size(), unread_buffer_size());
        message.append((char*)(buffer_->data()) + start_pos_, insert_size);
        start_pos_ += insert_size;
    }

    if (is_null == NullTerminator::kYes) {
        Assert(message.back() == '\0', "Message last character isn't a null terminator.")
        message.pop_back();
    }

    return message;
}

uint64_t
BufferReader::unread_buffer_size() {
    if(buffer_ == nullptr) {
        Assert(false, "BufferRead doesn't set buffer");
    }

    return buffer_->size() - start_pos_ - 1;
}

}

