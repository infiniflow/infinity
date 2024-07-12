// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <arpa/inet.h>
#include <boost/asio/write.hpp>

module buffer_writer;


import stl;
import third_party;
import pg_message;
import ring_buffer_iterator;

import infinity_exception;
import default_values;
import logger;

namespace infinity {

SizeT BufferWriter::size() const {
    const auto current_size = RingBufferIterator::Distance(start_pos_, current_pos_);
    return (current_size < 0) ? (current_size + PG_MSG_BUFFER_SIZE) : current_size;
}

void BufferWriter::send_string(const String &value, NullTerminator null_terminator) {
    auto position_in_string = 0u;

    if (!full()) {
        position_in_string = static_cast<u32>(std::min(max_capacity() - size(), value.size()));
        RingBufferIterator::CopyN(value.c_str(), position_in_string, current_pos_);
        current_pos_.increment(position_in_string);
    }

    while (position_in_string < value.size()) {
        const auto bytes_to_transfer = std::min(max_capacity(), value.size() - position_in_string);
        try_flush(bytes_to_transfer);
        RingBufferIterator::CopyN(value.c_str() + position_in_string, bytes_to_transfer, current_pos_);
        current_pos_.increment(bytes_to_transfer);
        position_in_string += bytes_to_transfer;
    }

    if (null_terminator == NullTerminator::kYes) {
        try_flush(sizeof(char));
        current_pos_.data_[current_pos_.position_] = NULL_END;
        current_pos_.increment();
    }
}

void BufferWriter::send_value_i8(i8 host_value) {
    try_flush(sizeof(i8));

    RingBufferIterator::CopyN(reinterpret_cast<const char *>(&host_value), sizeof(i8), current_pos_);
    current_pos_.increment(sizeof(i8));
}

void BufferWriter::send_value_u8(u8 host_value) {
    try_flush(sizeof(u8));

    RingBufferIterator::CopyN(reinterpret_cast<const char *>(&host_value), sizeof(u8), current_pos_);

    current_pos_.increment(sizeof(u8));
}

void BufferWriter::send_value_i16(i16 host_value) {
    try_flush(sizeof(i16));
    i16 network_value = htons(host_value);

    RingBufferIterator::CopyN(reinterpret_cast<const char *>(&network_value), sizeof(i16), current_pos_);
    current_pos_.increment(sizeof(i16));
}

void BufferWriter::send_value_u16(u16 host_value) {
    try_flush(sizeof(u16));
    u16 network_value = htons(host_value);

    RingBufferIterator::CopyN(reinterpret_cast<const char *>(&network_value), sizeof(u16), current_pos_);
    current_pos_.increment(sizeof(u16));
}

void BufferWriter::send_value_i32(i32 host_value) {
    try_flush(sizeof(i32));
    i32 network_value = htonl(host_value);

    RingBufferIterator::CopyN(reinterpret_cast<const char *>(&network_value), sizeof(i32), current_pos_);
    current_pos_.increment(sizeof(i32));
}

void BufferWriter::send_value_u32(u32 host_value) {
    try_flush(sizeof(u32));
    u32 network_value = htonl(host_value);

    RingBufferIterator::CopyN(reinterpret_cast<const char *>(&network_value), sizeof(u32), current_pos_);
    current_pos_.increment(sizeof(u32));
}

void BufferWriter::flush(SizeT bytes) {
    if (bytes > size()) {
        String error_message = "Can't flush more bytes than available";
        UnrecoverableError(error_message);
    }
    const auto bytes_to_send = bytes ? bytes : size();
    SizeT bytes_sent{0};

    boost::system::error_code boost_error;
    if ((RingBufferIterator::Distance(start_pos_, current_pos_) < 0)) {
        bytes_sent = boost::asio::write(
            *socket_,
            std::array<boost::asio::mutable_buffer, 2>{boost::asio::buffer(start_pos_.position_addr(), PG_MSG_BUFFER_SIZE - start_pos_.position_),
                                                       boost::asio::buffer(data_.begin(), current_pos_.position_)},
            boost::asio::transfer_at_least(bytes_to_send),
            boost_error);
    } else {
        bytes_sent = boost::asio::write(*socket_,
                                        boost::asio::buffer(start_pos_.position_addr(), size()),
                                        boost::asio::transfer_at_least(bytes_to_send),
                                        boost_error);
    }

    if (boost_error == boost::asio::error::broken_pipe || boost_error == boost::asio::error::connection_reset || bytes_sent == 0) {
        String error_message = fmt::format("Can't flush more bytes than available: {}", boost_error.message());
        UnrecoverableError(error_message);
    }

    if (boost_error) {
        UnrecoverableError(boost_error.message());
    }
    start_pos_.increment(bytes_sent);
}

void BufferWriter::try_flush(SizeT bytes) {
    if (bytes >= max_capacity() - size()) {
        flush(bytes);
    }
}

} // namespace infinity
