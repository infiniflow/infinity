//
// Created by JinHai on 2022/7/20.
//

module;

#include <boost/asio/read.hpp>
#include <arpa/inet.h>

import stl;
import pg_message;
import ring_buffer_iterator;
import infinity_assert;
import infinity_exception;
import default_values;

module buffer_reader;

namespace infinity {

SizeT BufferReader::size() const {
    const auto current_size = RingBufferIterator::Distance(start_pos_, current_pos_);
    return (current_size < 0) ? (current_size + PG_MSG_BUFFER_SIZE) : current_size;
}

String BufferReader::read_string() {
    auto end_pos = RingBufferIterator(data_);
    String result;

    if (size() != 0) {
        end_pos = RingBufferIterator::Find(start_pos_, current_pos_, NULL_END);
        RingBufferIterator::Copy(start_pos_, current_pos_, result);
        start_pos_.increment(result.size());
    }

    while (end_pos.dereference() != NULL_END) {
        receive_more();
        end_pos = RingBufferIterator::Find(start_pos_, current_pos_, NULL_END);
        RingBufferIterator::Copy(start_pos_, current_pos_, result);
        start_pos_ = end_pos;
    }

    // Skip the terminator marker
    start_pos_.increment();
    return result;
}

i8 BufferReader::read_value_i8() {
    receive_more(sizeof(i8));
    i8 network_value{0};

    RingBufferIterator::CopyN(start_pos_, sizeof(i8), reinterpret_cast<char *>(&network_value));
    start_pos_.increment(sizeof(i8));

    return network_value;
}

u8 BufferReader::read_value_u8() {
    receive_more(sizeof(u8));
    u8 network_value{0};

    RingBufferIterator::CopyN(start_pos_, sizeof(u8), reinterpret_cast<char *>(&network_value));
    start_pos_.increment(sizeof(u8));

    return network_value;
}

i16 BufferReader::read_value_i16() {
    receive_more(sizeof(i16));
    i16 network_value{0};

    RingBufferIterator::CopyN(start_pos_, sizeof(i16), reinterpret_cast<char *>(&network_value));
    start_pos_.increment(sizeof(i16));

    return ntohs(network_value);
}

u16 BufferReader::read_value_u16() {
    receive_more(sizeof(u16));
    u16 network_value{0};

    RingBufferIterator::CopyN(start_pos_, sizeof(u16), reinterpret_cast<char *>(&network_value));
    start_pos_.increment(sizeof(u16));

    return ntohs(network_value);
}

i32 BufferReader::read_value_i32() {
    receive_more(sizeof(i32));
    i32 network_value{0};

    RingBufferIterator::CopyN(start_pos_, sizeof(i32), reinterpret_cast<char *>(&network_value));
    start_pos_.increment(sizeof(i32));
    return ntohl(network_value);
}

u32 BufferReader::read_value_u32() {
    receive_more(sizeof(u32));
    u32 network_value{0};

    RingBufferIterator::CopyN(start_pos_, sizeof(u32), reinterpret_cast<char *>(&network_value));
    start_pos_.increment(sizeof(u32));
    return ntohl(network_value);
}

String BufferReader::read_string(const SizeT string_length, NullTerminator null_terminator) {
    String result;
    result.reserve(string_length);

    if (size() != 0) {
        RingBufferIterator::CopyN(start_pos_, Min(string_length, size()), result);
        start_pos_.increment(result.size());
    }

    while (result.size() < string_length) {
        const auto substring_length = Min(string_length - result.size(), max_capacity());
        receive_more(substring_length);
        RingBufferIterator::CopyN(start_pos_, substring_length, result);
        start_pos_.increment(substring_length);
    }

    if (null_terminator == NullTerminator::kYes) {
        Assert<NetworkException>(result.back() == NULL_END, "Last character isn't null.", __FILE_NAME__, __LINE__);
        result.pop_back();
    }

    return result;
}

void BufferReader::receive_more(SizeT bytes) {
    if (size() >= bytes) {
        // Current data in buffer is enough for reading.
        return;
    }

    // Get the available size of the buffer;
    const auto available_size = max_capacity() - size();

    SizeT bytes_read{0};

    boost::system::error_code boost_error;

    if((RingBufferIterator::Distance(start_pos_, current_pos_) < 0) || (start_pos_.position_ == 0)) {
        bytes_read = boost::asio::read(*socket_,
                                       boost::asio::buffer(current_pos_.position_addr(), available_size),
                                       boost::asio::transfer_at_least(bytes - size()),
                                       boost_error);
    } else {
        bytes_read = boost::asio::read(
                *socket_,
                std::array<boost::asio::mutable_buffer, 2>{boost::asio::buffer(current_pos_.position_addr(), PG_MSG_BUFFER_SIZE - current_pos_.position_),
                                                           boost::asio::buffer(&data_[0], start_pos_.position_ - 1)},
                boost::asio::transfer_at_least(bytes - size()),
                boost_error);
    }

    if (boost_error == boost::asio::error::broken_pipe || boost_error == boost::asio::error::connection_reset || bytes_read == 0) {
        Error<ClientException>("Client close the connection.", __FILE_NAME__, __LINE__);
    }

    Assert<NetworkException>(!boost_error, boost_error.message(), __FILE_NAME__, __LINE__);

    current_pos_.increment(bytes_read);
}

} // namespace infinity
