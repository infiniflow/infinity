//
// Created by JinHai on 2022/7/20.
//

#include "buffer_reader.h"

#include <boost/asio/read.hpp>
#include <iostream>

namespace infinity {

size_t
BufferReader::size() const {
    const auto current_size = std::distance(&*start_pos_, &*current_pos_);
    return (current_size < 0) ? (current_size + PG_MSG_BUFFER_SIZE) : current_size;
}

std::string
BufferReader::read_string() {
    auto end_pos = RingBufferIterator(data_);
    std::string result;

    if(size() != 0) {
        end_pos = std::find(start_pos_, current_pos_, NULL_END);
        std::copy(start_pos_, end_pos, std::back_inserter(result));
        std::advance(start_pos_, result.size());
    }

    while(*end_pos != NULL_END) {
        receive_more();
        end_pos = std::find(start_pos_, current_pos_, NULL_END);
        std::copy(start_pos_, end_pos, std::back_inserter(result));
        start_pos_ = end_pos;
    }

    ++start_pos_;
    return result;
}

std::string
BufferReader::read_string(const size_t string_length, NullTerminator null_terminator) {
    std::string result;
    result.reserve(string_length);

    if(size() != 0) {
        std::copy_n(start_pos_, std::min(string_length, size()), std::back_inserter(result));
        std::advance(start_pos_, result.size());
    }

    while(result.size() < string_length) {
        const auto substring_length = std::min(string_length - result.size(), max_capacity());
        receive_more(substring_length);
        std::copy_n(start_pos_, substring_length, std::back_inserter(result));
        std::advance(start_pos_, substring_length);
    }

    if(null_terminator == NullTerminator::kYes) {
        NetworkAssert(result.back() == NULL_END, "Last character isn't null.");
        result.pop_back();
    }

    return result;
}

void
BufferReader::receive_more(size_t bytes) {
    if(size() >= bytes) {
        // Current data in buffer is enough for reading.
        return;
    }

    // Get the available size of the buffer;
    const auto available_size = max_capacity() - size();

    size_t bytes_read{0};

    boost::system::error_code boost_error;

    if(std::distance(&*start_pos_, &*current_pos_) < 0 || &*start_pos_ == &data_[0]) {
        bytes_read = boost::asio::read(
                *socket_,
                boost::asio::buffer(&*current_pos_, available_size),
                boost::asio::transfer_at_least(bytes - size()),
                boost_error);
    } else {
        bytes_read = boost::asio::read(
                *socket_,
                std::array<boost::asio::mutable_buffer, 2>{
                        boost::asio::buffer(&*current_pos_, std::distance(&*current_pos_, data_.end())),
                        boost::asio::buffer(data_.begin(), std::distance(data_.begin(), &*start_pos_ - 1))
                },
                boost::asio::transfer_at_least(bytes - size()), boost_error);
    }

    if(boost_error == boost::asio::error::broken_pipe || boost_error == boost::asio::error::connection_reset ||
       bytes_read == 0) {
        ClientError("Client close the connection.");
    }

    NetworkAssert(!boost_error, boost_error.message());

    std::advance(current_pos_, bytes_read);
}


}

