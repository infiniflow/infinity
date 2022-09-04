//
// Created by JinHai on 2022/7/20.
//

#include "buffer_writer.h"
#include "session.h"

#include <boost/asio/write.hpp>

namespace infinity {

size_t
BufferWriter::size() const {
    const auto current_size = std::distance(&*start_pos_, &*current_pos_);
    return (current_size < 0) ? (current_size + PG_MSG_BUFFER_SIZE) : current_size;
}

void
BufferWriter::send_string(const std::string &value, NullTerminator null_terminator) {
    auto position_in_string = 0u;

    if(!full()) {
        position_in_string = static_cast<uint32_t>(std::min(max_capacity() - size(), value.size()));
        std::copy_n(value.begin(), position_in_string, current_pos_);
        std::advance(current_pos_, position_in_string);
    }

    while(position_in_string < value.size()) {
        const auto bytes_to_transfer = std::min(max_capacity(), value.size() - position_in_string);
        try_flush(bytes_to_transfer);
        std::copy_n(value.begin() + position_in_string, bytes_to_transfer, current_pos_);
        std::advance(current_pos_, bytes_to_transfer);
        position_in_string += bytes_to_transfer;
    }

    if(null_terminator == NullTerminator::kYes) {
        try_flush(sizeof (char));
        *current_pos_ = NULL_END;
        ++ current_pos_;
    }
}

void
BufferWriter::flush(size_t bytes) {
    NetworkAssert(bytes <= size(), "Can't flush more bytes than available");
    const auto bytes_to_send = bytes ? bytes : size();
    size_t bytes_sent;

    boost::system::error_code boost_error;
    if (std::distance(&*start_pos_, &*current_pos_) < 0) {
        bytes_sent = boost::asio::write(*socket_,
                                        std::array<boost::asio::mutable_buffer, 2> {
                                            boost::asio::buffer(&*start_pos_, std::distance(&*start_pos_, data_.end())),
                                            boost::asio::buffer(data_.begin(), std::distance(data_.begin(), &*current_pos_))
                                        },
                                        boost::asio::transfer_at_least(bytes_to_send), boost_error);

    } else {
        bytes_sent = boost::asio::write(*socket_, boost::asio::buffer(&*start_pos_, size()),
                                        boost::asio::transfer_at_least(bytes_to_send), boost_error);
    }

    if(boost_error == boost::asio::error::broken_pipe || boost_error == boost::asio::error::connection_reset || bytes_sent == 0) {
        NetworkAssert(false, "Write failed. Client close connection.");
    }

    NetworkAssert(!boost_error, boost_error.message());
    std::advance(start_pos_, bytes_sent);
}

void
BufferWriter::try_flush(size_t bytes) {
    if (bytes >= max_capacity() - size()) {
        flush(bytes);
    }
}

}
